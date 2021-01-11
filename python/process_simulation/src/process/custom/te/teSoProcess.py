"""
    The class TeSoProcess is a subclass of SoProcess and
    should implement the Tennessee Eastman process by running
    the simulation as a shared object.
    Since the shared object used by Nils is just doing some
    part we have to also implement the other part here.
    Our part is solve the derivation so all the other classes
    you can find here are to support solving and to get the
    shared object to run.
"""

from process.process import ProcessShutdown
from process.soProcess import SoProcess

import time
from ctypes import Structure, c_double, c_char, c_int, byref

#### from PI.py ####

class VelocityPI:
    def __init__(self, v_ic, kc, ts, ti):
        self.kc = kc
        self.ti = ti
        self.ts = ts
        self.lastErr = v_ic

    def increment(self, sp, meas):
        err = sp - meas
        out = (self.kc * (err + err*(self.ts / self.ti) - self.lastErr))
        self.lastErr = err
        return out

class DiscretePI:
    def __init__(self, ic, v_ic, kc, lo, hi, ts, ti):
        self.vpi = VelocityPI(v_ic, kc, ts, ti)
        self.lastDelta = ic
        self.kc = kc
        self.lo = lo
        self.hi = hi

    def saturation(self, hi, lo, val):
        if val < lo:
            return lo
        elif val > hi:
            return hi
        else:
            return val

    def increment(self, sp, meas, t):
        delta = self.vpi.increment(sp, meas)
        delta += self.lastDelta
        delta = self.saturation(self.hi, self.lo, delta)
        out = self.lastDelta
        self.lastDelta = delta
        return out

#### from ControlLoops.py ####

class PercentG_inProduct:

    def __init__(self, ic, v_ic, pos_R, neg_R, kc, lo, hi, ts, ti):
        self.dpi = DiscretePI(ic, v_ic, kc, lo, hi, ts, ti)
        self.pos_R = pos_R
        self.neg_R = neg_R

    def rateLimiter(self, sp, t):
        if t != 0.:
            rate = (sp - self.lastOut) / (t - self.lastTime)
            if rate > self.pos_R:
                out = (t - self.lastTime) * self.pos_R + self.lastOut
            elif rate < self.neg_R:
                out = (t - self.lastTime) * self.neg_R + self.lastOut
            else:
                out = sp
        else:
            out = sp
            self.lastTime = t
            self.lastOut = out

        return out

    def increment(self, sp, meas, t):
        self.pctGsp = self.rateLimiter(sp, t)
        return self.dpi.increment(self.pctGsp, meas, t)

    def getPctGsp(self):
        return self.pctGsp

class ProductionRate:

    def __init__(self, ic, v_ic, pos_R, neg_R, kc, lo, hi, ts, ti):
        self.dpi = DiscretePI(ic, v_ic, kc, lo, hi, ts, ti)
        self.pos_R = pos_R
        self.neg_R = neg_R

    def rateLimiter(self, sp, t):
        if t != 0.:
            rate = (sp - self.lastOut) / (t - self.lastTime)
            if rate > self.pos_R:
                out = (t - self.lastTime) * self.pos_R + self.lastOut
            elif rate < self.neg_R:
                out = (t - self.lastTime) * self.neg_R + self.lastOut
            else:
                out = sp
        else:
            out = sp
            self.lastTime = t
            self.lastOut = out

        return out

    def increment(self, sp, meas, t):
        prodRate = self.rateLimiter(sp, t)
        return ((prodRate * (100. / 22.89)) + self.dpi.increment(prodRate, meas, t))

#### from TEProcess.py ####

class ProcessValues(Structure):
    _fields_ = [("xmeas", c_double*41), ("xmeasadd", c_double*32),\
                ("xmeasdist", c_double*21), ("xmeasmonitor", c_double*62),\
                ("xmeascomp", c_double*96), ("xmv", c_double*12)]

class DistVector(Structure):
    _fields_ = [("idv", c_double*29)]

class RandState(Structure):
    _fields_ = [("g", c_double), ("measnoise", c_double),\
                ("procdist", c_double)]

class ComponentData(Structure):
    _fields_ = [("avp", c_double*8), ("bvp", c_double*8),\
                ("cvp", c_double*8),\
                ("ah", c_double*8), ("bh", c_double*8), ("ch", c_double*8),\
                ("ag", c_double*8), ("bg", c_double*8), ("cg", c_double*8),\
                ("av", c_double*8), ("ad", c_double*8), ("bd", c_double*8),\
                ("cd", c_double*8), ("xmw", c_double*8)]

class pProcess(Structure):
    _fields_ = [("uclr", c_double*8), ("ucvr", c_double*8),\
                ("utlr", c_double),\
                ("utvr", c_double), ("xlr", c_double*8),\
                ("xvr", c_double*8),\
                ("etr", c_double), ("esr", c_double), ("tcr", c_double), \
                ("tkr", c_double), ("dlr", c_double), ("vlr", c_double), \
                ("vvr", c_double), ("vtr", c_double), ("ptr", c_double), \
                ("ppr", c_double*8), ("crxr", c_double*8),\
                ("rr", c_double*4),\
                ("rh", c_double), ("fwr", c_double), ("twr", c_double),\
                ("qur", c_double), ("hwr", c_double), ("uar", c_double),\
                ("ucls", c_double*8), ("ucvs", c_double*8),\
                ("utls", c_double),\
                ("utvs", c_double), ("xls", c_double*8),\
                ("xvs", c_double*8),\
                ("ets", c_double), ("ess", c_double), ("tcs", c_double),\
                ("tks", c_double), ("dls", c_double), ("vls", c_double),\
                ("vvs", c_double), ("vts", c_double), ("pts", c_double),\
                ("pps", c_double*8), ("fws", c_double), ("tws", c_double),\
                ("qus", c_double), ("hws", c_double), ("uclc", c_double*8),\
                ("utlc", c_double), ("xlc", c_double*8), ("etc", c_double),\
                ("esc", c_double), ("tcc", c_double), ("dlc", c_double),\
                ("vlc", c_double), ("vtc", c_double), ("quc", c_double),\
                ("ucvv", c_double*8), ("utvv", c_double),\
                ("xvv", c_double*8),\
                ("etv", c_double), ("esv", c_double), ("tcv", c_double),\
                ("tkv", c_double), ("vtv", c_double), ("ptv", c_double),\
                ("vcv", c_double*12), ("vrng", c_double*12),\
                ("vtau", c_double*12),\
                ("ftm", c_double*13), ("fcm", c_double*104),\
                ("xst", c_double*104),\
                ("xmws", c_double*13), ("hst", c_double*13),\
                ("tst", c_double*13),\
                ("sfr", c_double*8), ("cpflmx", c_double),\
                ("cpprmx", c_double),\
                ("cpdh", c_double), ("tcwr", c_double),\
                ("tcws", c_double),\
                ("htr", c_double*3), ("agsp", c_double),\
                ("xdel", c_double*41),\
                ("xdeladd", c_double*24), ("xns", c_double*41),\
                ("xnsadd", c_double*34),\
                ("tgas", c_double), ("tprod", c_double),\
                ("vst", c_double*12), ("ivst", c_double*12)]

class RandProcess(Structure):
    _fields_ = [("adist", c_double*20), ("bdist", c_double*20),\
                ("cdist", c_double*20),\
                ("ddist", c_double*20), ("tlast", c_double*20),\
                ("tnext", c_double*20),\
                ("hspan", c_double*20), ("hzero", c_double*20),\
                ("sspan", c_double*20),\
                ("szero", c_double*20), ("spspan", c_double*20),\
                ("idvwlk", c_double*20), ("rdumm", c_double)]

class stModelData(Structure):
    """ This class is used by the shared object to read and write values. """

    _fields_ = [("pv_", ProcessValues), ("dvec_", DistVector),\
                ("randsd_", RandState), ("const_", ComponentData),\
                ("teproc_", pProcess), ("wlk_", RandProcess),\
                ("msg", c_char*256), ("code_sd", c_double),\
                ("tlastcomp", c_double), ("MSFlag", c_int)]

class TeSoProcess(SoProcess):

    # Naming taken from Nils project report.
    # NOTE! We only have 50 values instead of 41+12=53, because some are sensor and actor in one.
    stateNames = [
            # Sensor values (XMEAS 1-41)
            "A and C feed (stream 4) | XMEAS(4)",
            "Recycle flow (stream 8) | XMEAS(5)",
            "Reactor feed rate (stream 6) | XMEAS(6)",
            "Reactor pressure | XMEAS(7)",
            "Reactor level | XMEAS(8)",
            "Reactor temperature | XMEAS(9)",
            "Purge rate (stream 9) | XMEAS(10)",
            "Product separator temperature | XMEAS(11)",
            "Product separator level | XMEAS(12)",
            "Product separator pressure | XMEAS(13)",
            "Product separator underflow (stream 10) | XMEAS(14)",
            "Stripper level | XMEAS(15)",
            "Stripper pressure | XMEAS(16)",
            "Stripper underflow (stream 11) | XMEAS(17)",
            "Stripper temperature | XMEAS(18)",
            "Stripper steam flow | XMEAS(19)",
            "Compressor work | XMEAS(20)",
            "Reactor cooling water outlet temperature | XMEAS(21)",
            "Separator cooling water outlet temperature | XMEAS(22)",
            "A reactor feed analysis | XMEAS(23)",
            "B reactor feed analysis | XMEAS(24)",
            "C reactor feed analysis | XMEAS(25)",
            "D reactor feed analysis | XMEAS(26)",
            "E reactor feed analysis | XMEAS(27)",
            "F reactor feed analysis | XMEAS(28)",
            "A purge gas analysis | XMEAS(29)",
            "B purge gas analysis | XMEAS(30)",
            "C purge gas analysis | XMEAS(31)",
            "D purge gas analysis | XMEAS(32)",
            "E purge gas analysis | XMEAS(33)",
            "F purge gas analysis | XMEAS(34)",
            "G purge gas analysis | XMEAS(35)",
            "H purge gas analysis | XMEAS(36)",
            "D product analysis | XMEAS(37)",
            "E product analysis | XMEAS(38)",
            "F product analysis | XMEAS(39)",
            "G product analysis | XMEAS(40)",
            "H product analysis | XMEAS(41)",
            # Actor values (XMV 1-12)
            "D feed flow (stream 2) | XMV(1) | XMEAS(2)",
            "E feed flow (stream 3) | XMV(2) | XMEAS(3)",
            "A feed flow (stream 1) | XMV(3) | XMEAS(1)",
            "A and C feed flow (stream 4) | XMV(4)",
            "Compressor recycle valce | XMV(5)",
            "Purge valve (stream 9) | XMV(6)",
            "Separator pot liquid flow (stream 10) | XMV(7)",
            "Stripper liquid product flow (stream 11) | XMV(8)",
            "Stripper steam valve | XMV(9)",
            "Reactor cooling water flow | XMV(10)",
            "Condenser cooling water flow | XMV(11)",
            "Agitator speed | XMV(12)"
    ]

    @property
    def state(self):
        """ Just return self.ModelData.pv_.xmeas (XMEAS values) combined with self.rx (XMV values) as state..easier then change stuff everywhere. """
        return [self.ModelData.pv_.xmeas[i] for i in range(41)] + [self.rx[i] for i in range(38,50)]

    @state.setter
    def state(self, newState):
        """ Just set rx. """
        for i in range(12):
            self.rx[38+i] = newState[41+i]

    def __init__(self, config):
        """
            This method initialises the Tennessee Eastman Process with a
            shared object backend

            Args:
                config (dict): Configurations for the process.
        """
        super().__init__(config)

        # Use this object to transfer data from an to our shared object.
        self.ModelData = stModelData()

        # number of states
        self.NX = c_int(50)
        self.rt = c_double()
        # stores the current state variables
        # NOTE! c_double*50 creates a array like class..no idea how this is implemented
        self.rx = (c_double*50)()
        # stores the current derivatives
        self.dx = (c_double*50)()
        # timestamp
        self.h = 0.0005
        self.shut = 0
        self.start = time.time()
        self.iteration = 0

        # init values for plcs.
        self.t = 0.0
        self.scan_p = 0.0005

        # Initialize shared object simulation.
        
        # call init method of process simulation (shared object).
        # arg1: void* ModelData, get reference of self.ModelData object
        # arg2: const integer* nn, gets reference of self.NX c_int(50)
        self.simulation.teinit(byref(self.ModelData), byref(self.NX), 
                           byref(self.rt), self.rx,\
                           self.dx, None, None)

        # Set initial values for controlled values.
        initValues = [62.8065594603242, 53.2860269669116, 26.6612540091053,\
               60.4848010831412, 4.40000000000000e-323, 24.2346888157974,\
               37.2091027669474, 46.4308923440716, 8.20000000000000e-322,\
               35.9445883694097, 12.2095478800146, 99.9999999999999]
        for i in range(len(initValues)):
            self.rx[38+i] = initValues[i]

        # Register control loops.
        self.controlLoops = [
            lambda state : self.loop1(state),
            lambda state : self.loop2(state),
            lambda state : self.loop3(state),
            lambda state : self.loop4(state),
            lambda state : self.loop5(state)
        ]

    def simulate(self):
        super().simulate()

        # increase time by 0.0005 every round.
        self.rt = c_double(self.rt.value + self.h)

        # Update ModelData (callflag 1).
        self.simulation.tefunc(byref(self.ModelData), byref(self.NX), byref(self.rt), self.rx, self.dx, 1)
        # Update dx (callflag 2).
        ret = self.simulation.tefunc(byref(self.ModelData), byref(self.NX), byref(self.rt), self.rx, self.dx, 2)

        # Check for process exceptions.
        if ret != 0:
            raise ProcessShutdown(self.ModelData.msg.decode("utf-8"))

        # Update rx values.
        for i in range(self.NX.value):
            self.rx[i] = self.rx[i] + (self.h * self.dx[i])

        # update time. Skip first round to first init.
        if self.round != 1:
            self.t += 0.0005


############# PLC control loops. ##################

    def loop1(self, state):
        """ This method implements the logic for plc1. """
        # Init loop if needed.
        if self.round < 1:
            # NOTE! Just took these from Nils implementations.
            xmv_init = [62.8065594603242, 53.2860269669116, 26.6612540091053,\
                    60.4848010831412, 4.40000000000000e-323, 24.2346888157974,\
                    37.2091027669474, 46.4308923440716, 8.20000000000000e-322,\
                    35.9445883694097, 12.2095478800146, 99.9999999999999]
            r_init = [0.0027228362442929, 36.452848893533, 44.538708121546,\
                 0.092303538366845, 0.001915509802465, 0.253705005583181,\
                 0.228711771174511]

            # initialize r
            self.r = [x for x in r_init]
            # loop 13
            self.pctG_inProduct = PercentG_inProduct(0.632250244508519,-0.506656747709997,(0.3*22.95 / 24.),(-0.3*22.95 / 24.),-0.4, float("-inf"),float("inf"), self.scan_p,(100. / 60.))
            # loop 14
            self.yA_ControlLoop = VelocityPI(0.156799893651460, 2.0e-4, 0.1, 1.)
            # loop 15
            self.yAC_ControlLoop = VelocityPI(-0.193592358293763, 3.0e-4, 0.1, 2.)
            # loop 1
            self.A_FeedRateLoop = DiscretePI(xmv_init[2], 0.002694050999489,0.01, 0., 100., self.scan_p, (0.001 / 60.))
            # loop 2
            self.D_FeedRateLoop = DiscretePI(xmv_init[0], 62.429570964992760,1.6e-6, 0., 100., self.scan_p,(0.001 / 60.))
            # loop 3
            self.E_FeedRateLoop = DiscretePI(xmv_init[1], 9.723386274665245,1.8e-6, 0., 100., self.scan_p,(0.001 / 60.))
            # loop 4
            self.C_FeedRateLoop = DiscretePI(xmv_init[3], 0.083514558990949,0.003, 0., 100., self.scan_p,(0.001 / 60.))
            # loop 8
            self.productionRate = ProductionRate(-0.448883361482739,-0.140664345075038,(0.3*22.95 / 24.),(-0.3*22.95 / 24.), 3.2, -30.,30., self.scan_p, (120. / 60.))
            # loop 16
            self.ReactorTemperatureLoop= DiscretePI(xmv_init[9],0.007961389832360, -8.0, 0.,100., self.scan_p, (7.5 / 60.))
        else:
            # NOTE! Value taken from Nils.
            yac_sp = 51.
            ya_sp = 63.1372
            pctg_sp = 53.8
            reactor_temp_sp = 122.9
            prod_rate_sp = 22.89
            # Calculate new xmv values
            # Update the mole % of G in the product
            E_Adj = self.pctG_inProduct.increment(pctg_sp, state[39], self.t)

            # Update the A and C Measurements
            yAC = state[22] + state[24]
            yA = (state[22] * 100.) / yAC
            # Update the ratio trimmming and the yA and yAC loops (every 0.1 of t)
            tMod = self.t % 0.1
            if (tMod < 0.00005) or (0.1 - tMod) < 0.00005 or self.t == 0:
                loop14 = self.yA_ControlLoop.increment(ya_sp, yA)
                loop15 = self.yAC_ControlLoop.increment(yac_sp, yAC)
                if (self.t == 0):
                    self.r0_unitDelay_out = 0.002722836244293
                    self.r3_unitDelay_out = 0.092236609900323
                    self.r0_unitDelay_store = loop14 + self.r0_unitDelay_out
                    self.r3_unitDelay_store = (-1. * loop14) + loop15 + self.r3_unitDelay_out
                else:
                    self.r0_unitDelay_out = self.r0_unitDelay_store
                    self.r3_unitDelay_out = self.r3_unitDelay_store
                    self.r0_unitDelay_store = loop14 + self.r0_unitDelay_out
                    self.r3_unitDelay_store = (-1. * loop14) + loop15 + self.r3_unitDelay_out
                self.r[0] = loop14 + self.r0_unitDelay_out
                self.r[3] = (-1. * loop14) + loop15 + self.r3_unitDelay_out
            # Update the production rate
            self.Fp = self.productionRate.increment(prod_rate_sp, state[16], self.t)
            # Update the feedforward
            mP_G_SP = self.pctG_inProduct.getPctGsp()
            self.r[1] = 1.5192e-3 * mP_G_SP**2. + 5.9446e-1 * mP_G_SP + 2.7690e-1 - ((E_Adj * 32.) / self.Fp)
            self.r[2] = -1.1377e-3 * mP_G_SP**2. + -8.0893e-1 * mP_G_SP + 9.1060e1 + ((E_Adj * 46.) / self.Fp)
            # Update the feed rate loops
            xmv3 = self.A_FeedRateLoop.increment((self.r[0] * self.Fp),state[0], self.t)
            xmv1 = self.D_FeedRateLoop.increment((self.r[1] * self.Fp),state[1], self.t)
            xmv2 = self.E_FeedRateLoop.increment((self.r[2] * self.Fp),state[2], self.t)
            xmv4 = self.C_FeedRateLoop.increment((self.r[3] * self.Fp),state[3], self.t)
            # Update the reactor temperature loop
            xmv10 = self.ReactorTemperatureLoop.increment(reactor_temp_sp,state[8], self.t)
            state[41+0] = xmv1
            state[41+1] = xmv2
            state[41+2] = xmv3
            state[41+3] = xmv4
            state[41+9] = xmv10
        return state

    def loop2(self, state):
        """ This method implements the logic for plc2. """
        # Init loop if needed.
        if self.round < 1:
            # loop 11
            self.ReactorLevelLoop = DiscretePI(92.020048977831070,0.040751615684570, 0.8,0., 120., self.scan_p, (60. / 60.))
            # loop 17
            self.SeparatorTemperatureLoop = DiscretePI(12.2095478800146,-0.141566616033629,-4.0, 0., 100.,self.scan_p, (15. / 60.))
        else:
            reactor_level_sp = 65.0
            sepTempSP = self.ReactorLevelLoop.increment(reactor_level_sp,state[7], self.t)
            xmv11 = self.SeparatorTemperatureLoop.increment(sepTempSP, state[10],self.t)
            state[41+10] = xmv11
        return state

    def loop3(self, state):
        """ This method implements the logic for plc3. """
        # Init loop if needed.
        if self.round < 1:
            # loop 8
            self.productionRate = ProductionRate(-0.448883361482739,-0.140664345075038,(0.3*22.95 / 24.),(-0.3*22.95 / 24.), 3.2, -30.,30., self.scan_p, (120. / 60.))
            # loop 6
            self.SeparatorFlowLoop = DiscretePI(37.2091027669474,0.048094569533532,4.0e-4, 0., 100.,self.scan_p,(0.001 / 60.))
            # loop 10
            self.SeparatorLevelLoop = DiscretePI(0.253705005583181,-1.041770047054492,-1.0e-3, 0., 100., self.scan_p,(200. / 60.))
        else:
            sep_level_sp = 50.0
            prod_rate_sp = 22.89
            Fp = self.productionRate.increment(prod_rate_sp, state[16], self.t)
            r5 = self.SeparatorLevelLoop.increment(sep_level_sp, state[11], self.t)
            xmv7 = self.SeparatorFlowLoop.increment((r5 * Fp), state[13], self.t)
            state[41+6] = xmv7
        return state

    def loop4(self, state):
        """ This method implements the logic for plc4. """
        # Init loop if needed.
        if self.round < 1:
            # loop 5
            self.PurgeRateLoop = DiscretePI(24.2346888157974, -0.006360143307706,0.01, 0., 100., self.scan_p,(0.001 / 60.))
            # loop 8
            self.productionRate = ProductionRate(-0.448883361482739,-0.140664345075038,(0.3*22.95 / 24.),(-0.3*22.95 / 24.), 3.2, -30.,30., self.scan_p, (120. / 60.))
            # loop 12
            self.ReactorPressureLoop = DiscretePI(0.001915509802465,1.040853861903997,-1.0e-4, 0., 100.,self.scan_p, (20. / 60.))
        else:
            reactor_pressure_sp = 2800.0
            prod_rate_sp = 22.89
            Fp = self.productionRate.increment(prod_rate_sp, state[16], self.t)
            r4 = self.ReactorPressureLoop.increment(reactor_pressure_sp,state[6], self.t)
            xmv6 = self.PurgeRateLoop.increment((r4 * Fp), state[9], self.t)
            state[41+5] = xmv6
        return state

    def loop5(self, state):
        """ This method implements the logic for plc5. """
        # Init loop if needed.
        if self.round < 1:
            # loop 8
            self.productionRate = ProductionRate(-0.448883361482739,-0.140664345075038,(0.3*22.95 / 24.),(-0.3*22.95 / 24.), 3.2, -30.,30., self.scan_p, (120. / 60.))
            # loop 7
            self.StripperFlowLoop = DiscretePI(46.4308923440716,0.077381965807636,4.0e-4, 0., 100., self.scan_p,(0.001 / 60.))
            # loop 9
            self.StripperLevelLoop = DiscretePI(0.228711771174511,0.949174713615896,-2.0e-4, 0., 100., self.scan_p,(200. / 60.))
        else:
            stripper_level_sp = 50.0
            prod_rate_sp = 22.89
            Fp = self.productionRate.increment(prod_rate_sp, state[16], self.t)
            r6 = self.StripperLevelLoop.increment(stripper_level_sp, state[14],self.t)
            xmv8 = self.StripperFlowLoop.increment((r6 * Fp), state[16], self.t)
            state[41+7] = xmv8
        return state
