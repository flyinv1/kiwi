"""
"""
import numpy as np
from . import mach

def thrust(pc, pe, pa, k, At, Ae):
    """
        Compute thrust given chamber properties and nozzle geometry

        Arguments:
            pc : chamber pressure (Pa)
            pe : exit pressure (Pa)
            pa : ambient pressure (Pa)
            k : specific heat ratio
            At : nozzle throat area (m2)
            Ae : nozzle exit area (m2)

        Returns:
            F : thrust (N)
    """
    return pc * At * np.sqrt(2 * pow(k, 2) / (k - 1) * pow(2 / (k + 1), (k + 1) / (k - 1)) * (1 - pow(pe / pc, (k-1) / k))) + (pe / pa) * Ae


def thrust_from_cf(cf, pc, At):
    """
        Compute thrust from thrust coefficient

        Arguments:
            cf : nozzle thrust coefficient
            pc : chamber pressure (Pa)
            At : nozzle throat area

        Returns:
            F : thrust (N)
    """
    return cf * At / pc


def pe_from_pc(pc, Ar, k):
    """
        Compute exit pressure from chamber pressure and area ratio

        Arguments:
            pc : chamber pressure (Pa)
            Ae : nozzle area ratio
            k : chamber specific heat ratio

        Returns:
            pe : exit pressure (Pa)
    """
    Me, _ = mach.machFromAreaRatio(AR=Ar, k=k, maxiter=300)
    return pc * pow((1 + 1 / 2 * (k - 1) * pow(Me, 2)), k / (1 - k))


def thrustCoefficient(pc, pe, Ar, k, pa = 0):
    """
        Compute nozzle thrust coefficient from chamber conditions and nozzle geometry

        Arguments:
            pc : chamber pressure (Pa)
            pe : exit pressure (Pe)
            Ar : nozzle area ratio
            k : specific heat ratio
        
        Optional Arguments:
            pa [0] : ambient pressure (Pa)

        Returns:
            Cf : nozzle thrust coefficient
    """
    pe = pe_from_pc(pc, Ar, k)
    return np.sqrt(2 * pow(k, 2) / (k - 1) * pow(2 / (k + 1), (k + 1) / (k - 1)) * (1 - pow(pe / pc, (k - 1) / k))) + (pe / pc - pa / pc) * Ar 