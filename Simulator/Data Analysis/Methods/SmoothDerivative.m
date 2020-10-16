function [t, xf, xdot] = SmoothDerivative(t, x, filter_mode, derivative_mode, varargin):

    opts = struct('period', 1)

    if mod(length(varargin)) ~= 0
        error('SmoothDerivative: invalid name-value pair in arguments')
    args = reshape(varargin, 2, [])

    xf = []
    xdot = []
    switch filter_mode
    case 'moving-average'

    case 'hampel'
        xf = hampel(x)
    otherwise
        exception = MException('SmoothDerivative: invalid filter option', 'Option %s not available. Valid options are "moving-average", ""', filter) 
        throw(exception)

    