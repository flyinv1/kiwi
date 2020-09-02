import React, { useEffect, useRef, useState, useMemo } from 'react';
import Plot, { StreamLine, Color, Line, Themes, Axes, Grid } from './gl-rtplot';
import styles from './GLPlot.module.scss';

const delta = 1;

const GLPlot = ({ 
    className,
    animate = false,
    newData,
    configuration,
    width,
    height
 }) => {

    let canvas = useRef();
    let glplot = useRef(null);

    let _newData = useRef();
    let _dataAvailable = useRef(false);
    let _animate = useRef(false);

    /**
     * Plot initialization effect, requires context attributes and grid
     */
    useEffect(
        () => {

            const { contextAttributes } = configuration;

            let plot = new Plot(canvas.current, contextAttributes);

            if (configuration?.layout?.axes) {
                let axes = new Axes();
                axes.color = configuration?.layout?.axes?.color || new Color(0.75, 0.75, 0.75, 1.0);
                plot.setAxes(axes);
            };

            if (configuration?.layout?.grid) {
                let grid = new Grid(
                    configuration?.layout?.grid?.xInterval || 1,
                    configuration?.layout?.grid?.yInterval || 1
                );
                grid.majorColor = configuration?.layout.grid?.color || new Color(0.85, 0.85, 0.85, 1.0);
                plot.setGrid(grid);
            }

            plot.setLimits(
                configuration?.layout?.limits?.xmin || 0,
                configuration?.layout?.limits?.xmax || 10,
                configuration?.layout?.limits?.ymin || -5,
                configuration?.layout?.limits?.ymax || 5,
            );

            if (!configuration?.series) {
                console.warn('GLPlot: no series specified');
            }

            Object.keys(configuration?.series || {}).map(key => {
                const series = configuration.series[key];
                if (series) {

                    let streamline = new StreamLine(
                        series?.duration || 30,
                        series?.points || 200
                    );

                    // TODO:: Improve default color options
                    streamline.color = series?.color || new Color(0.5, 0.5, 0.5, 1.0);

                    plot.addSeries(
                        key,
                        streamline,
                    )
                }
            })

            plot.render();

            glplot.current = plot;

            return () => {
                glplot.current.dispose();
            };
        },
        [ configuration ]
    );

    useEffect(() => {
        _newData.current = newData;
        _dataAvailable.current = true;
        
    }, [ newData ])

    useEffect(
        () => {
            // Update plot size
            if (glplot.current) {
                glplot.current.resize();
            }
        },
        [ width, height ]
    );

    useEffect(
        () => {

            _animate.current = animate;

            const animateFrame = (t) => {

                if (_dataAvailable.current) {
                    Object.keys(_newData.current).map(key => {
                        const series = glplot.current.series?.[key];
                        if (series) series.shiftIn(_newData.current[key], t)
                    })
                    _dataAvailable.current = false;
                } else {
                    Object.values(glplot.current.series).map(series => {
                        if (series) series.shiftIn(new Float32Array([0]), t);
                    })
                }

                glplot.current.render();

                // animate frame code
                if (_animate.current) requestAnimationFrame(animateFrame);
            };

            if (animate) animateFrame(0);
        },
        [ animate ]
    );

    return (
        <div className={[styles.container, className].join(' ')}>
            { configuration?.layout?.axes && 
                <>
                    <div className={styles.x_axes}>
                        <div>{configuration?.layout?.limits?.xmin || '0'}</div>
                        <div>{configuration?.layout?.limits?.xmax || ''}</div>
                    </div>
                    <div className={styles.y_axes}>
                        <div>{configuration?.layout?.limits?.ymin || '0'}</div>
                        <div>{(configuration?.layout?.limits?.ymax + configuration?.layout?.limits?.ymin) / 2}</div>
                        <div>{configuration?.layout?.limits?.ymax || ''}</div>
                    </div>
                </>
            }
            <canvas
                ref={canvas}
                width={width}
                height={height}
                className={[ styles.canvas ].join(' ')}
            />
        </div>
    );
};

export default GLPlot;
