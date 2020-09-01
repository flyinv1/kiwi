export const limits = (xmin, xmax, ymin, ymax) => {

    const _delta = {
        x: xmax - xmin,
        y: ymax - ymin
    };

    const _scale = {
        x: 2 / _delta.x,
        y: 2 / _delta.y
    }

    const _origin = {
        x: xmin + _delta.x / 2,
        y: ymin + _delta.y / 2,
    }

    return({
        _scale,
        _origin
    })
}