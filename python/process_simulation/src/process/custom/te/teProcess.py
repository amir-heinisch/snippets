"""

This file implements the tennesse eastman process simulation in python.

# TODO: Still in development..

"""

from process.process import Process

class TeProcess(Process):

    def __init__(self):
        """ Reimplementation of teinit in python. """

        # TODO: import class definitions.
        # tcModelData = ModelData()

        # TODO: No idea why we should adjust the pointers here?!
        #--yp
        #--yy

        # tcModelData.MSFlag stuff
        # NOTE! MSFlag was always None in Nils implementation -> so we use 0.
        MSFlag = 0

        ### ComponentData struct values. ###
        xmw = [2.0, 25.4, 28.0, 32.0, 46.0, 48.0,62.0,76.0] # in g/mol / kg/kmol

        avp = [0.0, 0.0, 0.0, 15.92, 16.35, 16.35, 16.43, 17.21] # in ln(Torr)
        bvp = [0.0, 0.0, 0.0, -1444.0, -2144.0, -2144.0, -2748.0, -3318.0] # in ln(Torr)*°C
        cvp = [0.0, 0.0, 0.0, 258.0, 265.5, 265.5, 232.9, 249.6] # in °C

        ad = [1.0, 1.0, 1.0, 23.3, 33.9, 32.8, 49.5, 50.5] # in ?
        bd = [0.0, 0.0, 0.0, -0.07, -0.0957, -0.0995, -0.0191, -0.0541] # in ?
        cd = [0.0, 0.0, 0.0, -2e-4, -1.52e-4, -2.33e-4, -4.25e-4, -1.5e-4] # in ?

        ah = [1e-6, 1e-6, 1e-6, 9.6e-7, 5.73e-7, 6.52e-7, 5.15e-7, 4.71e-7] # in ?
        bh = [0.0, 0.0, 0.0, 8.7e-9, 2.41e-9, 2.18e-9, 5.65e-10, 8.7e-10] # in ?
        ch = [0.0, 0.0, 0.0, 4.81e-11, 1.82e-11, 1.94e-11, 3.82e-12, 2.62e-12] # in ?

        av = [1e-6, 1e-6, 1e-6, 8.67e-5, 1.6e-4, 1.6e-4, 2.25e-4, 2.09e-4] # in ?
        ag = [3.411e-6, 3.799e-7, 2.491e-7, 3.567e-7, 3.465e-7, 3.93e-7, 1.7e-7, 1.5e-7] # in ?
        bg = [7.18e-10, 1.08e-9, 1.36e-11, 8.51e-10, 8.96e-10, 1.02e-9, 0.0, 0.0] # in ?
        cg = [6e-13, -3.98e-13, -3.93e-14, -3.12e-13, -3.27e-13, -3.12e-13, 0.0, 0.0] # in ?
        ### Set initial sensor values (rx)?! ###
        # Initial state of processs.
        yy_01_36 = []
        # Cooling water outlet temperatures.
        yy_37_38 = []
        # Valve positions.
        yy_39_50 = []

        ### Init teproc object values. ###
        noteproc_pv_xmv = yy_39_50
        vcv = yy_39_50
        vst = [2.0] * 12 # in %
        ivst = [0.0] * 12 # in 1

        # Nominal flowrate through valves.
        vrng = [400.0, 400.0, 100.0, 1500.0, 1500.0, 1e3.0, 0.03, 1e3, 1200.0]
        # Volumes of vessels.
        vtr = 1300.0
        vts = 3500.0
        vtc = 156.5
        vtv = 5e3

        htr = [0.06899381054, 0.05]
        hwr = 7060.0
        hws = 11138.0
        sfr = [0.995, 0.991, 0.99, 0.916, 0.936, 0.938, 0.058, 0.0301]

        # Stream 2.
        xst_01_07 = [] # in mol%
        tst[0] = 45.0 # °C
        # Steam 3.
        xst_08_15 = [] # in mol%
        tst[1] = 45.0 # °C
        # Stream 1.
        xst_16_23 = [] # in mol%
        tst[2] = 45.0 # in °C
        # Stream 4.
        xst_24_31 = [] # in mol%
        tst[3] = 45.0 # in °C

        cpflmx = 280275.0 # in ?
        cpprmx = 1.3 # in ?

        # Time constatns of valves.
        vtau = [8.0, 8.0, 6.0, 9.0, 7.0, 5.0, 5.0, 5.0, 120.0, 5.0, 5.0, 5.0] # in sec
        for i in range(12):
            vtau[i-1] /= 3600.0

        # Seed the random generator.
        # NOTE! rseed was never passed from niclas so just hardcode values.
        randsd_g = 1431655765.0
        randsd_measnoise = 1431655765.0
        randsd_procdist = 1431655765.0

        # NOTE MSFlag is always 0 in our simulation..
        xns = [0.0012, 18.0, 22.0, 0.05, 0.2, 0.21, 0.3, 0.5, 0.01, 0.0017, 0.01, 1.0, 0.3, 0.125, 1.0, 0.3, 0.115, 0.01, 1.15, 0.2, 0.01, 0.01, 0.25, 0.1, 0.25, 0.1, 0.25, 0.025, 0.25, 0.1, 0.25, 0.1, 0.25, 0.025, 0.05, 0.05, 0.01, 0.01, 0.01, 0.5, 0.5]
        xnsadd = [0.01, 0.01, ]

        # Init distrubance flags.
        dvec_idv = [0.0] * 28 # in 1

        # Data of disturbance processes.
        # TODO: 1-18 are missing.
        wlk_hspan[19] = 0.1 # in h
        wlk_hzero[19] = 0.2 # in h
        wlk_sspan[19] = 60.0 # in gpm
        wlk_szero[19] = 1200.0 # in gpm
        wlk_spspan[19] = 0.0 # in gpm / h

        tlastcomp = -1.0
        
        # TODO: calls simulation
        # NOTE: uses Callflag = 0
        tefunc()

        ### Overwrite rx/valve positions again..why? ###
        xmv = [62.8065594603242, 53.2860269669116, 26.6612540091053,\
               60.4848010831412, 4.40000000000000e-323, 24.2346888157974,\
               37.2091027669474, 46.4308923440716, 8.20000000000000e-322,\
               35.9445883694097, 12.2095478800146, 99.9999999999999]
        for i in range(12):
            self.rx[38+i] = xmv[i]

    def simulate(self):
        """
            This method runs the next iteration of the process simulation.
        """

        super().simulate()

        # update time
        self.rt = c_double(self.rt.value + self.h)

        # call tefunc to update ModelData (note that callflag = 1)
        self.teproc.tefunc(byref(self.ModelData), byref(self.NX),\
                           byref(self.rt), self.rx, self.dx, 1)

        # call tefunc to update dx (note that callflag = 2)
        retValue = self.teproc.tefunc(byref(self.ModelData), byref(self.NX),\
                           byref(self.rt), self.rx, self.dx, 2)
        print retValue
	#plotting
	meas = self.get_xmeas()
	with open('plot.txt','a') as f:
	    f.write(str(self.iteration)+','+str(meas[6])+'\n')
		
		
        self.iteration = self.iteration + 1
        print self.iteration
        if (retValue != 0):
            if (self.shut == 0):
                current = sys.stdout
                sys.stdout = sys.__stdout__
                print "Shut-Down occurred"
                sys.stdout = current
                self.shut = 1
                print time.time() - self.start

        # euler to update rx
        self.euler()

        # get new xmv values
        for i in range(12):
            self.cursor.execute("SELECT value FROM te_proc WHERE name = \
                                 'XMV%d';" % (i+1))
            xmv = self.cursor.fetchall()
            self.rx[38+i] = float(xmv[0][0])

        # submit new xmeas values
        for i in range(self.ModelData.pv_.xmeas._length_):
            self.cursor.execute("UPDATE te_proc SET value = %lf WHERE name = \
                                 'XMEAS%d';" % (self.ModelData.pv_.xmeas[i],\
                                 i+1))

        self.conn.commit()

    def runControlLoops(self, state=None):
        pass

    def euler(self):
        # compute next state 
        for i in range(self.NX.value):
            k = self.h * self.dx[i]
            self.rx[i] = self.rx[i] + k

    def get_xmeas(self):
        xmeas = [self.ModelData.pv_.xmeas[i] for i in range(self.ModelData.pv_.xmeas._length_)]
        return xmeas

    def get_state(self):
        return self.rx

"""

    # Plc1 logic
    XMV = [1, 2, 3, 4, 10]

    def plc1_update(self):
        # DEBUG: state before plc1

        # NOTE! Not needed, since attacker has full control about all values in this setting.
        # Read values for these sensors.
        XMEAS = [1, 2, 3, 4, 7, 8, 9, 23, 25]
        # Send values from these sensors to other plcs.
        XMEAS_SEND = [7, 8]
        # Recieve values from plc5.
        XMEAS_RECEIVE = [17, 40]

        ## Calculate new actor values. ##

        # Update the mole % of G in the product
        E_Adj = self.pctG_inProduct.increment(pctg_sp, xmeas40, self.t)

        # Update the A and C Measurements
        yAC = xmeas23 + xmeas25
        yA = (xmeas23 * 100.) / yAC

        # DEBUG: state after plc1


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
"""
