import React, { useEffect, useRef, useState } from 'react';
import Plot, { Color, Line, Themes, Axes } from './gl-rtplot';
import styles from './GLPlot.module.scss';

const defaultConfig = {
    points: 10,
    streams: 1,
    axes: true,
    duration: 0
};

const defaultLayout = {
    scale: 1,
    pixelRatio: 1,
    maintainAspect: false
};

const defaultContextAttributes = {
    antialias: true,
    alpha: true
}

const GLPlot = ({ 
    config = defaultConfig,
    layout = defaultLayout,
    contextAttributes = defaultContextAttributes,
    buffer = [],
    isLive = true,
    width,
    height,
    colors,
    className
 }) => {
    let canvas = useRef();
    let glplot = useRef(null);

    // Utilized during animate frame
    let _buffer = useRef([]);
    let _t = useRef(0);
    let _dt = useRef(16.67);
    let live = useRef(true);

    /**
     * Plot initialization effect, requires context attributes and grid
     */
    useEffect(
        () => {
            let plot = new Plot(canvas.current, {
                antialias: contextAttributes?.antialias || false,
                alpha: contextAttributes?.alpha || true
            });

            plot.renderer.setPixelRatio(2);

            console.log(plot);

            if (config.axes) {
                let majorColor = new Color(0.85, 0.85, 0.85, 1.0);
                let minorColor = new Color(0.95, 0.95, 0.95, 1.0);

                let axes = new Axes(majorColor);

                plot.attachAxes(axes);
            }

            glplot.current = plot;

            return () => {
                glplot.current.dispose();
            };
        },
        [ contextAttributes.antialias, contextAttributes.alpha, config.axes ]
    );

    useEffect(
        () => {
            _buffer.current = buffer;
        },
        [ buffer ]
    );

    useEffect(
        () => {
            let dx = 1 / config.points;
            if (glplot.current) {
                let plot = glplot.current;
                for (let i = 0; i < config.streams; i++) {
                    let colors = Themes.palette.midnight;
                    let line = new Line(Color.fromHex(colors[i % (colors.length - 1)], 1.0), config.points + 1);
                    line.fill(0, dx, 0);
                    plot.addStream(line);
                }
                console.log("should render...")
                plot.render();
                glplot.current = plot;
            }
        },
        [ config.streams, config.points ]
    );

    useEffect(
        () => {
            _dt.current = config.duration / config.points;
        },
        [ config.points, config.duration ]
    );

    useEffect(
        () => {
            // Update plot size
            if (glplot.current) {
                glplot.current.renderer.setSize(width, height);
            }
        },
        [ width, height ]
    );

    useEffect(
        () => {
            if (glplot.current) {
                glplot.current.renderer.maintainAspect = layout.maintainAspect;
            }
        },
        [ layout.maintainAspect ]
    );

    useEffect(
        () => {
            // Update plot color values
        },
        [ colors ]
    );

    useEffect(
        () => {
            glplot.current.renderer.setPixelRatio(layout.pixelRatio);
        },
        [ layout.pixelRatio ]
    );

    useEffect(
        () => {
            if (glplot.current && layout.scale) {
                glplot.current.setScale(layout.scale, layout.scale);
            }
        },
        [ layout.scale ]
    );

    useEffect(
        () => {
            live.current = isLive;

            const animateFrame = (t) => {
                let dt = t - _t.current;

                let intervals = Math.floor(dt / _dt.current);
                let remainder = dt % _dt.current;

                if (glplot.current && dt >= _dt.current) {
                    glplot.current.lines.map((line, i) => {
                        for (let j = 0; j < intervals; j++) {
                            line.push(_buffer.current.slice(i, i + 1));
                        }
                    });
                    glplot.current.render();
                    _t.current = t - remainder;
                }

                // glplot.current.render();

                // animate frame code
                if (live.current) requestAnimationFrame(animateFrame);
            };
            
            if (isLive) animateFrame(0);
        },
        [ isLive ]
    );

    return (
        <div className={className}>
            <canvas
                ref={canvas}
                width={width || 300}
                height={height || 300}
                className={[ styles.canvas ].join(' ')}
            />
        </div>
    );
};

GLPlot.propTypes = {
    // config: PropTypes.objectOf({
    //     scale: PropTypes.objectOf({
    //         x: PropTypes.number,
    //         y: PropTypes.number
    //     })
    // }),
    // streams: PropTypes.number.isRequired,
    // points: PropTypes.number.isRequired,
    // duration: PropTypes.number.isRequired,
    // colors: PropTypes.array,
    // width: PropTypes.number,
    // height: PropTypes.number
};

export default GLPlot;
