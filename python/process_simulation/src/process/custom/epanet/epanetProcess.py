"""
    The epanet simulation can run in two modes. One simulates the hydraulics
    and the other the water quality in a water pipe network.

    This class just run the owa epanet (https://github.com/OpenWaterAnalytics/EPANET)
    via the python wrapper called owa-epanet from this
    repo: https://github.com/OpenWaterAnalytics/epanet-python.
    an alternative backend you can try is:
    https://github.com/USEPA/WNTR

    So the .inp file contains a network definition.
    The state the attacker see is the combination of
    the node and link values (the first ones are read only
    but the attacker dont nows this it just does not change
    anythink).

    It is possible to implement some controles and rules
    via the input file but we should implement them
    like the loops in the te process..

"""

from process.process import ProcessShutdown
from process.soProcess import Process

import os
import time
import logging
import epanet.toolkit as en

class EpanetProcess(Process):

    # Holds mode to run: 0=Hydraulic, 1=WaterQuality, 2=Both
    mode = 0

    # Holds the project reference.
    project = None

    # Holds the epanet network graph
    networkGraph = {}

    # Holds the names for the values in state.
    stateNames = []

    @property
    def state(self):
        state = []
        # Get number of nodes and links.
        nodeCount = en.getcount(ph=self.project, object=en.NODECOUNT)
        linkCount = en.getcount(ph=self.project, object=en.LINKCOUNT)

        # Enumerate all nodes.
        for index in range(1,nodeCount+1):
            # Create state from current values.
            state.append(en.getnodevalue(ph=self.project, index=index, property=en.TANKLEVEL))
            state.append(en.getnodevalue(ph=self.project, index=index, property=en.HEAD))
            state.append(en.getnodevalue(ph=self.project, index=index, property=en.DEMAND))
            state.append(en.getnodevalue(ph=self.project, index=index, property=en.PRESSURE))
        for index in range(1,linkCount+1):
            state.append(en.getlinkvalue(ph=self.project, index=index, property=en.FLOW))
            state.append(en.getlinkvalue(ph=self.project, index=index, property=en.VELOCITY))
            state.append(en.getlinkvalue(ph=self.project, index=index, property=en.HEADLOSS))
            state.append(en.getlinkvalue(ph=self.project, index=index, property=en.STATUS))
            state.append(en.getlinkvalue(ph=self.project, index=index, property=en.PUMP_STATE))
            state.append(en.getlinkvalue(ph=self.project, index=index, property=en.ENERGY))
        return state


    @state.setter
    def state(self, newState):
        node_num = len(self.getNodeNames())
        for i in range(node_num):
            pass
            #en.setnodevalue(ph=self.project, index=index, property=en.DIAMETER, value=newState[i])
        for i in range(node_num, len(self.stateNames)):
            #en.setlinkvalue(ph=self.project, index=index, property=en.ELEVATION, value=newState[i])
            pass


    def __init__(self, config):
        """ Init object. """

        super().__init__(config)

        # Process given config.
        # TODO: handle wrong file format or not existent files.
        try:
            self.inputPath = config['epanetInputFile']
            self.rptFile = config['epanetReportFile']
            self.outFile = config['epanetOutputFile']
        except:
            logging.error("Epanet process needs special configuration inputFile, reportFile, outputFile.")

        # Setup epanet base.
        self.setupEpanet()

        # Init state.
        self.state = []

        # Init network graph dict.
        self.networkGraph['n_x'], self.networkGraph['n_y'], \
        self.networkGraph['n_z'] = self.getNodeCoordinates()
        self.networkGraph['e_x'], self.networkGraph['e_y'], \
        self.networkGraph['e_z'] = self.getLinkCoordinates()

        # Setup statusNames.
        self.stateNames = self.getNodeNames() + self.getLinkNames()

        # Register control loops.
        self.registerControlLoops()

        # Draw info image.
        self.infoImage()


    def simulate(self):
        # Increase duration to run infinite.
        cd = en.gettimeparam(ph=self.project, param=en.DURATION)
        step = en.gettimeparam(ph=self.project, param=en.HYDSTEP)
        en.settimeparam(ph=self.project, param=en.DURATION, value=cd+step)

        th = 1

        try:
            # Run simulation at given time/round.
            en.runH(ph=self.project)
            # Check time left.
            tH = en.nextH(ph=self.project)

        except Exception as e:
            # TODO: do stuff.
            logging.error(str(e))

        if tH <= 0:
            raise ProcessShutdown("Epanet hydraulic simulation shutdown. (Epanet internal time: " + str(ct) + "seconds)")

        # NOTE! Epanet starts at round zero.
        super().simulate()


    def setupEpanet(self):
        """ This method sets up the epanet project. """
        # Create epanet project.
        self.project = en.createproject()
        # Read network data from input file.
        en.open(ph=self.project, inpFile=self.inputPath, rptFile=self.rptFile, outFile=self.outFile)
        # Open and init hydaulic solver.
        en.openH(ph=self.project)
        en.initH(ph=self.project, initFlag=en.NOSAVE)


    def registerControlLoops(self):
        """
            This method retrieves and deletes all controls from epanet
            and implements them here. This allows the attacker to tamper
            with them later.
        """
        self.controlLoops = []

        count = en.getcount(ph=self.project, object=en.CONTROLCOUNT)
        for idx in range(1,count+1):
            typ, linkIndex, setting, nodeIndex, level = en.getcontrol(ph=self.project, index=idx)
            if typ == en.LOWLEVEL:
                #self.controlLoops.append(lambda state, level=level, node=nodeIndex: if state[node] < level:
            elif typ == en.HILEVEL:
                pass
                # TODO: implement
            elif typ == en.TIMER:
                pass
                # TODO: implement
            elif typ == en.TIMEOFDAY:
                pass
                # TODO: implement
            else:
                loggin.error("Control type unknown.")


    def infoImage(self):
        """
            Create image with additional information for process.
        """
        try:
            import plotly.graph_objects as go
            from plotly.graph_objs import Scatter

            # Create nodes and edges.
            node_x, node_y, node_z = self.networkGraph['n_x'], self.networkGraph['n_y'], self.networkGraph['n_z']
            edge_x, edge_y, edge_z = self.networkGraph['e_x'], self.networkGraph['e_y'], self.networkGraph['e_z']
            edge_trace = go.Scatter3d(x=edge_x, y=edge_y, z=edge_z, hoverinfo='none', mode='lines')
            node_trace = go.Scatter3d(x=node_x, y=node_y, z=node_z, mode='markers')

            # Create figure and write.
            # TODO use path from config.
            fig = go.Figure(data=[edge_trace, node_trace])
            fig.update_layout(showlegend=False)
            filePath = 'out/epanetRunner.html'
            if os.path.isfile(filePath):
                os.remove("out/epanetRunner.html")
            fig.write_html("out/epanetRunner.html")
        except Exception as e:
            logging.error(e)


    def getNodeNames(self):
        names = []
        nodeCount = en.getcount(ph=self.project, object=en.NODECOUNT)

        for idx in range(nodeCount):
            index = idx + 1
            # Get node id.
            nodeId = en.getnodeid(ph=self.project, index=index)
            # Get node type.
            nodeType = "Node"
            if en.getnodetype(ph=self.project, index=index) == en.TANK:
                nodeType = "Tank"
            elif en.getnodetype(ph=self.project, index=index) == en.JUNCTION:
                nodeType = "Junction"
            elif en.getnodetype(ph=self.project, index=index) == en.RESERVOIR:
                nodeType = "Reservoir"
            else:
                nodeType = "Unknown"
            names.append(nodeType + "-" + nodeId + " (tanklevel)")
            names.append(nodeType + "-" + nodeId + " (head)")
            names.append(nodeType + "-" + nodeId + " (demand)")
            names.append(nodeType + "-" + nodeId + " (pressure)")

        return names


    def getLinkNames(self):
        names = []
        linkCount = en.getcount(ph=self.project, object=en.LINKCOUNT)

        for idx in range(linkCount):
            # TODO: add headCurveIndex = en.getheadcurveindex(ph=self.project, id=""), getPumpType, getVertex, getVertexCount
            index = idx + 1
            # Get link id.
            linkId = en.getlinkid(ph=self.project, index=index)
            # Get start and end node.
            linkNodes = en.getlinknodes(ph=self.project, index=index)
            sNode = en.getcoord(ph=self.project, index=linkNodes[0])
            sNode.append(en.getnodevalue(ph=self.project, index=linkNodes[0], property=en.ELEVATION))
            eNode = en.getcoord(ph=self.project, index=linkNodes[1])
            eNode.append(en.getnodevalue(ph=self.project, index=linkNodes[1], property=en.ELEVATION))
            # Get link type.
            linkType = en.getlinktype(ph=self.project, index=index)
            if linkType == en.CVPIPE:
                linkType = "Cvpipe"
            elif linkType == en.PIPE:
                linkType = "Pipe"
            elif linkType == en.PUMP:
                linkType = "Pump"
            elif linkType == en.PRV:
                linkType = "Prv"
            elif linkType == en.PSV:
                linkType = "Psv"
            elif linkType == en.PBV:
                linkType = "Pbv"
            elif linkType == en.FCV:
                linkType = "Fcv"
            elif linkType == en.TCV:
                linkType = "Tcv"
            elif linkType == en.GPV:
                linkType = "Gpv"
            else:
                linkType = "Unknown"
            names.append(linkType + "-" + linkId + " (flow)")
            names.append(linkType + "-" + linkId + " (velocity)")
            names.append(linkType + "-" + linkId + " (headloss)")
            names.append(linkType + "-" + linkId + " (status)")
            names.append(linkType + "-" + linkId + " (pump_state)")
            names.append(linkType + "-" + linkId + " (energy)")

        return names


    def getNodeCoordinates(self):
        c_x, c_y, c_z = [], [], []
        nodeCount = en.getcount(ph=self.project, object=en.NODECOUNT)
        for idx in range(nodeCount):
            index = idx + 1
            coords = en.getcoord(ph=self.project, index=index)
            coords.append(en.getnodevalue(ph=self.project, index=index, property=en.ELEVATION))
            c_x.append(coords[0])
            c_y.append(coords[1])
            c_z.append(coords[2])

        return c_x, c_y, c_z


    def getLinkCoordinates(self):
        c_x, c_y, c_z = [], [], []
        linkCount = en.getcount(ph=self.project, object=en.LINKCOUNT)

        for idx in range(linkCount):
            index = idx + 1
            # Get start and end node.
            linkNodes = en.getlinknodes(ph=self.project, index=index)
            sNode = en.getcoord(ph=self.project, index=linkNodes[0])
            sNode.append(en.getnodevalue(ph=self.project, index=linkNodes[0], property=en.ELEVATION))
            eNode = en.getcoord(ph=self.project, index=linkNodes[1])
            eNode.append(en.getnodevalue(ph=self.project, index=linkNodes[1], property=en.ELEVATION))

            c_x.append(sNode[0]), c_y.append(sNode[1]), c_z.append(sNode[2])
            c_x.append(eNode[0]), c_y.append(eNode[1]), c_z.append(eNode[2])
            c_x.append(None), c_y.append(None), c_z.append(None)

        return c_x, c_y, c_z


    def __del__(self):
        # Close solver.
        en.closeH(ph=self.project)
        # Close project.
        en.close(ph=self.project)
