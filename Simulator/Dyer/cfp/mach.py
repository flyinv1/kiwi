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
    # Compute the mach number from area ratio using the Newton-Rhapson method

    # Validate inputs
    if not isinstance(k, float):
        raise Exception(f'Specific heat ratio must be of type float, received type(k) = {type(k)}')
    elif (k < 1.0):
        raise Exception(f'Specific heat ratio must be greater than 1')

    if not isinstance(AR, (float, int)):
        raise Exception(f'Area ratio must be of type float or int, received type(AR) = {type(AR)}')
    elif (AR < 1.0):
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