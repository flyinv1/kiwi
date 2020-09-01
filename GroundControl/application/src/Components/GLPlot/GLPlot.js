import React, { useEffect, useRef, useState } from 'react';
import Plot, { Color, Line, Themes, Axes } from './gl-rtplot';
import styles from './GLPlot.module.scss';
import StreamLine from './gl-rtplot/src/StreamLine';

const delta = 16;

const defaultConfig = {
    points: 10,
    streams: 1,
    axes: true,
    duration: 0
};

const defaultContextAttributes = {
    antialias: true,
    alpha: true
}

const GLPlot = ({ 
    config = defaultConfig,
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

            if (config.axes) {

                let axes = new Axes(majorColor);
                axes.grid(10, 10, 1, 1);

                plot.addAxes(axes);
            }

            plot.setLimits(-10 , 60, -2, 2);

            plot.addSeries('test', new StreamLine(60, 3600));
            plot.addSeries('test0', new StreamLine(60, 3600));


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
            if (glplot.current) {
                let plot = glplot.current;
                let colors = Themes.palette.midnight;

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
                // glplot.current.renderer.setSize(width, height);
            }
        },
        [ width, height ]
    );

    useEffect(
        () => {
            // Update plot color values
        },
        [ colors ]
    );

    useEffect(
        () => {
            live.current = isLive;

            const animateFrame = (t) => {

                if (t - _t.current > delta) {

                    glplot.current.series['test'].shiftIn(new Float32Array([Math.sin(t / 300)]), t);
                    glplot.current.series['test0'].shiftIn(new Float32Array([Math.sin(t / 300 + 1)]), t);

                    _t.current = t;

                } else {
                    glplot.current.series['test'].update(t);
                    glplot.current.series['test0'].update(t);
                }

                glplot.current.render();

                // glplot.current.series['test'].update(t);

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
