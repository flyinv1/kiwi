from math import exp
from scipy.optimize import newton, brentq 

def areaRatioFromMach(M, k):
    ar = 0
    try:
        ar = 1 / M * pow((2 + (k - 1) * pow(M, 2)) / (k + 1), (k + 1) / (2 * (k - 1)))
    except Exception as exception:
        # Placeholder
        raise exception
    return ar


def derivativeAreaRatioFromMach(M, k):
    dar = 0
    try:
        dar = pow(((k - 1) * pow(M, 2) + 2) / (k + 1), ((k+1) / (2 * (k-1))) - 1)- pow((((k - 1) * pow(M, 2) + 2) / (k + 1)), (k+1) / (2 * (k - 1))) / pow(M, 2)
    except Exception as exception:
        # Placeholder
        raise exception
    return dar


def machFromAreaRatio(AR=1.0, k=1.4, maxiter=100, tol=1e-8):
    """
        Compute exit mach number from chamber pressure and area ratio

        Arguments:
            pc : chamber pressure (Pa)
            Ar : nozzle area ratio
            k : chamber specific heat ratio

        Returns:
            (supersonic, subsonic) : tuple of exit mach numbers
    """
    # Compute the mach number from area ratio using the Newton-Rhapson method

    if k < 1.0:
        raise Exception(f'Specific heat ratio must be greater than 1')

    if AR < 1.0:
        raise Exception(f'Area ratio must be greater than 1')

    suproot = None
    subroot = None

    try:
        suproot = newton(lambda m : areaRatioFromMach(m, k) - AR, 2, lambda m : derivativeAreaRatioFromMach(m, k), maxiter=maxiter, tol=tol)
        subroot = newton(lambda m : areaRatioFromMach(m, k) - AR, 0.1, lambda m : derivativeAreaRatioFromMach(m, k), maxiter=maxiter, tol=tol)
    except RuntimeError:
        try:
            suproot = newton(lambda m : areaRatioFromMach(m, k) - AR, 2, maxiter=maxiter, tol=tol)
            subroot = newton(lambda m : areaRatioFromMach(m, k) - AR, 0.1, maxiter=maxiter, tol=tol)
        except RuntimeError as rtError:
            raise rtError

    return (suproot, subroot)