import CoreObject from './Core/CoreObject';
import OBJECT from './Core/Objects';
import Color from './Core/Color';
import Line from './Line';

class StreamLine extends Line {

    constructor(duration, points) {
        super(points);
        this.duration = duration;
        this.time = 0;

        // Populate the vector with 0's
        this.fill(0, duration / points, 0);
    }
    
    /**
     * @description Shift existing data according to elapsed time, this is done to achieve smooth animation
     * Though data may not update at 60fps
     * @param {*} time 
     */
    update(time) {
        const dt = (time - this.time) * 0.001;
        for (let i = 0; i < this.bufferSize; i++) {
            this.setX(i, this.getX(i) - dt);
        }
        this.time = time;
    }

    /**
     * @description Shift data in according to time
     * @param {Float32Array} data new data buffer
     * @param {Number} time current time (millis)
     */
    shiftIn(data, time) {
        const dt = (time - this.time) * 0.001;
        const len = data.length;

        for (let i = 0; i < this.bufferSize - len; i++) {
            this.setX(i, this.getX(i + len) - dt);
            this.setY(i, this.getY(i + len));
        }

        // Push the new data to the end of the buffer
        for (let i = 0; i < len; i++) {
            this.setX(i + this.bufferSize - len, this.duration)
            this.setY(i + this.bufferSize - len, data[i]);
        }

        this.time = time;
    }

}

export default StreamLine;