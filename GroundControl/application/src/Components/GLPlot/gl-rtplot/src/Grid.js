import Line from './Line';

/**
 * 
 * @param {Color} majorColor 
 * @param {Color} minorColor 
 * @param {Number} [xIntervals = 10] xIntervals 
 * @param {Number} [yIntervals = 10] yIntervals 
 * @param {Number} [xMajorIntervals = 0] xMajorIntervals 
 * @param {Number} [yMajorIntervals = 0] yMajorIntervals
 */
function Grid(majorColor, minorColor, xIntervals = 10, yIntervals = 10, xMajorInterval = 0, yMajorInterval = 0) {
    this.static = true;
    this.enabled = true;

    this.x = {
        intervals: xIntervals,
        majorInterval: xMajorInterval,
        children: []
    };

    this.y = {
        intervals: xIntervals,
        majorInterval: yMajorInterval,
        children: []
    };

    this.setXInterval = (intervals, majorInterval) => {
        this.x.majorInterval = majorInterval;
        this.x.interals = intervals;
        this.x.children = [];
        for (let i = -intervals; i < intervals + 1; i++) {
            let color = i % majorInterval === 0 && majorInterval > 0 ? majorColor : minorColor;
            let line = new Line(color, 2);
            line.buffer = new Float32Array([ i / intervals, -1, i / intervals, 1 ]);
            this.x.children.push(line);
        }
    };

    this.setYInterval = (intervals, majorInterval) => {
        this.y.majorInterval = majorInterval;
        this.y.interals = intervals;
        this.y.children = [];
        for (let i = -intervals; i < intervals + 1; i++) {
            let color = i % majorInterval === 0 && majorInterval > 0 ? majorColor : minorColor;
            let line = new Line(color, 2);
            line.buffer = new Float32Array([ -1, i / intervals, 1, i / intervals ]);
            this.y.children.push(line);
        }
    };

    this.setXInterval(xIntervals, xMajorInterval);
    this.setYInterval(yIntervals, yMajorInterval);
}

export default Grid;
