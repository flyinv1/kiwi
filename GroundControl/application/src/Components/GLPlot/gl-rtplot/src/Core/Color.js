class Color {
    /**
     * @property {Number} r - Red component (0 - 1)
     */
    r;
    
    /**
     * @property {Number} g - Green component (0 - 1)
     */
    g;

    /**
     * @property {Number} b - Green component (0 - 1)
     */
    b;

    /**
     * @property {Number} a - Alpha component (0 - 1)
     */
    a;

    /**
     * 
     * @param {Number} r - Red (0 - 1)
     * @param {Number} g - Green (0 - 1)
     * @param {Number} b - Blue (0 - 1)
     * @param {Number} a - Alpha (0 - 1)
     */
    constructor(r, g, b, a) {
        this.r = r;
        this.g = g;
        this.b = b;
        this.a = a;
    }

    /**
     * @description Returns a BufferColor from hex string and alpha value
     * @param {String} hex - 3 or 6 value hex code
     * @param {Number} alpha - alpha value from 0 to 1
     */
    static fromHex(hex, alpha) {
        // Expand shorthand form (e.g. "03F") to full form (e.g. "0033FF")
        let result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result ? new Color(
            parseInt(result[1], 16) / 256,
            parseInt(result[2], 16) / 256,
            parseInt(result[3], 16) / 256,
            alpha ? alpha : 1.0,
        ) : false;
      }
}

export default Color;