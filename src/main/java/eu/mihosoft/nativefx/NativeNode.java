/*
 * Copyright 2019-2019 Michael Hoffer <info@michaelhoffer.de>. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * If you use this software for scientific research then please cite the following publication(s):
 *
 * M. Hoffer, C. Poliwoda, & G. Wittum. (2013). Visual reflection library:
 * a framework for declarative GUI programming on the Java platform.
 * Computing and Visualization in Science, 2013, 16(4),
 * 181–192. http://doi.org/10.1007/s00791-014-0230-y
 */
/*
 * This Java source file was generated by the Gradle 'init' task.
 */
package eu.mihosoft.nativefx;

import java.awt.BasicStroke;
import java.lang.annotation.Native;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.util.Date;

import javax.swing.border.StrokeBorder;

import eu.mihosoft.nativefx.NativeBinding.IntEnum;
import eu.mihosoft.nativefx.NativeBinding.MODIFIER;
import eu.mihosoft.nativefx.NativeBinding.MOUSE_BTN;
import javafx.animation.Animation;
import javafx.animation.AnimationTimer;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.geometry.Rectangle2D;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.control.Labeled;
import javafx.scene.image.ImageView;
import javafx.scene.image.PixelBuffer;
import javafx.scene.image.PixelFormat;
import javafx.scene.image.WritableImage;
import javafx.scene.image.WritablePixelFormat;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.Region;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.util.Callback;

/**
 * This node renders native buffers. It handles the connection to a
 * shared memory object, the transfer of input events and resize requests as
 * well as native events.
 */
public final class NativeNode extends Region {

    private String serverName;

    private final WritablePixelFormat<IntBuffer> formatInt 
        = PixelFormat.getIntArgbPreInstance();
        private final WritablePixelFormat<ByteBuffer> formatByte 
        = PixelFormat.getByteBgraPreInstance();

    private WritableImage img;
    private ImageView view;

    private ByteBuffer buffer;
    private IntBuffer intBuf;
    private PixelBuffer<ByteBuffer> pixelBuffer;

    private AnimationTimer timer;
    private int key = -1;

    private int buttonState;

    private boolean lockingError = false;

    private boolean hidpiAware = false;

    private int numValues = 10;
    private double[] fpsValues = new double[numValues];
    private long frameTimestamp;
    private int fpsCounter = 0;

    private boolean verbose;
    private boolean pixelBufferEnabled;

    /**
     * Constructor. Creates a new instance of this class without hidpi-awareness.
     */
    public NativeNode() {
        this(false, false);
    }

    /**
     * Constructor. Creates a new instance of this class.
     * @param hidpiAware determines whether this node should be hidpi-aware
     */
    public NativeNode(boolean hidpiAware, boolean pixelBufferEnabled) {

        this.hidpiAware = hidpiAware;
        this.pixelBufferEnabled = pixelBufferEnabled;
        
        addEventHandler(MouseEvent.MOUSE_MOVED, (ev)-> {

            double sx = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;
            double sy = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;

            double x = ev.getX()*sx;
            double y = ev.getY()*sy;
            
            long timestamp = System.nanoTime();

            NativeBinding.fireMouseMoveEvent(key, x, y,
                MOUSE_BTN.fromEvent(ev), MODIFIER.fromEvent(ev),
                timestamp
            );

            // ev.consume();
        });

        addEventHandler(MouseEvent.MOUSE_PRESSED, (ev)-> {
            double sx = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;
            double sy = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;

            double x = ev.getX()*sx;
            double y = ev.getY()*sy;
            
            long timestamp = System.nanoTime();

            buttonState = MOUSE_BTN.fromEvent(ev);

            NativeBinding.fireMousePressedEvent(key, x, y,
                MOUSE_BTN.fromEvent(ev), MODIFIER.fromEvent(ev),
                timestamp
            );

            // ev.consume();
        });

        addEventHandler(MouseEvent.MOUSE_RELEASED, (ev)-> {
            double sx = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;
            double sy = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;

            double x = ev.getX()*sx;
            double y = ev.getY()*sy;
            
            long timestamp = System.nanoTime();

            NativeBinding.fireMouseReleasedEvent(key, x, y,
                // TODO 11.07.2019 check whether we correctly detected btn up (what about multiple btns?)
                buttonState, MODIFIER.fromEvent(ev),
                timestamp
            );

            // ev.consume();
        });

        addEventHandler(MouseEvent.MOUSE_DRAGGED, (ev)-> {
            double sx = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;
            double sy = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;

            double x = ev.getX()*sx;
            double y = ev.getY()*sy;
            
            long timestamp = System.nanoTime();

            NativeBinding.fireMouseMoveEvent(key, x, y,
                MOUSE_BTN.fromEvent(ev), MODIFIER.fromEvent(ev),
                timestamp
            );

            // ev.consume();
        });

        addEventHandler(MouseEvent.MOUSE_CLICKED, (ev)-> {

            requestFocus();

            double sx = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;
            double sy = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;

            double x = ev.getX()*sx;
            double y = ev.getY()*sy;
            
            long timestamp = System.nanoTime();

            NativeBinding.fireMouseClickedEvent(key, x, y,
                MOUSE_BTN.fromEvent(ev), MODIFIER.fromEvent(ev),
                ev.getClickCount(),
                timestamp
            );

            // ev.consume();
        });

        addEventHandler(ScrollEvent.SCROLL, (ev)-> {
            double sx = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;
            double sy = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;

            double x = ev.getX()*sx;
            double y = ev.getY()*sy;
            
            long timestamp = System.nanoTime();

            NativeBinding.fireMouseWheelEvent(key, x, y, ev.getDeltaY(),
                MOUSE_BTN.fromEvent(ev), MODIFIER.fromEvent(ev),
                timestamp
            );

            // ev.consume();
        });

        // ---- keys
        
        this.setFocusTraversable(true); // TODO make this optional?

        addEventHandler(KeyEvent.KEY_PRESSED, (ev)-> {

            // System.out.println("KEY: pressed " + ev.getText() + " : " + ev.getCode());
            
            long timestamp = System.nanoTime();

            NativeBinding.fireKeyPressedEvent(key, ev.getText(), ev.getCode().getCode(),
                /*modifiers*/0,
                timestamp
            );

            // ev.consume();
        });
        addEventHandler(KeyEvent.KEY_RELEASED, (ev)-> {

            // System.out.println("KEY: released " + ev.getText() + " : " + ev.getCode());
            
            long timestamp = System.nanoTime();

            NativeBinding.fireKeyReleasedEvent(key, ev.getText(), ev.getCode().getCode(),
                /*modifiers*/0,
                timestamp
            );

            // ev.consume();
        });
        addEventHandler(KeyEvent.KEY_TYPED, (ev)-> {

            // System.out.println("KEY: typed    " + ev.getText() + " : " + ev.getCode());
            
            long timestamp = System.nanoTime();

            NativeBinding.fireKeyTypedEvent(key, ev.getText(), ev.getCode().getCode(),
                /*modifiers*/0,
                timestamp
            );

            // ev.consume();
        });


        showNotConnectedText();

    }

    /**
     * Connects this node to the specified shared memory object.
     * @param name name of the shared memory to connect to
     * @throws RuntimeException if the connection cannot be established
     */
    public void connect(String name) {
        this.serverName = name;

        NativeBinding.init();

        disconnect();

        if(key <0 || NativeBinding.isConnected(key)) {
            key = NativeBinding.connectTo(name);
        }

        if(key <0) {
            showErrorText();
            throw new RuntimeException("Cannot connect to shared memory " + key + ".");
        }

        view = new ImageView();
        view.setPreserveRatio(false);

        view.fitWidthProperty().bind(widthProperty());
        view.fitHeightProperty().bind(heightProperty());

        Runnable r = () -> {

            long currentTimeStamp = System.nanoTime();

            // try to lock the shared resource
            lockingError = !NativeBinding.lock(key);
            if(lockingError) {
                showErrorText();
                timer.stop();
                return;
            }

            boolean dirty = NativeBinding.isDirty(key);
            boolean isReady = NativeBinding.isBufferReady(key);

            // if(!isReady) {
            //     System.out.println("["+key+"]> WARNING: buffer ready: " + isReady);
            // }

            NativeBinding.processNativeEvents(key);

            // if not dirty yet and/or not ready there's nothing
            // to do. we return early.
            if (!dirty || !isReady) {
                NativeBinding.unlock(key);
                return;
            }

            int currentW = NativeBinding.getW(key);
            int currentH = NativeBinding.getH(key);

            // create new image instance if the image doesn't exist or
            // if the dimensions do not match
            if (img == null || Double.compare(currentW, img.getWidth()) != 0
                    || Double.compare(currentH, img.getHeight()) != 0) {

                // System.out.println("  -> resize W: " + currentW + ", H: " + currentH);

                
                buffer = NativeBinding.getBuffer(key);
                intBuf = buffer.order(ByteOrder.LITTLE_ENDIAN).asIntBuffer();

                if(pixelBufferEnabled) {
                    pixelBuffer = new PixelBuffer<>(currentW, currentH, buffer, formatByte);
                    img = new WritableImage(pixelBuffer);
                } else {
                    img = new WritableImage(currentW, currentH);
                }

                view.setImage(img);
            }

            if(pixelBufferEnabled) {
                pixelBuffer.updateBuffer(b->new Rectangle2D(0,0,currentW, currentH));
            } else {
                img.getPixelWriter().setPixels(0, 0, (int) img.getWidth(), (int) img.getHeight(), formatInt, intBuf,
                        (int) img.getWidth());
            }

                // we updated the image, not dirty anymore
                // NativeBinding.lock(key);
                NativeBinding.setDirty(key, false);

                int w = (int)getWidth();
                int h = (int)getHeight();

                double sx = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;
                double sy = hidpiAware?getScene().getWindow().getRenderScaleX():1.0;

                if((w != NativeBinding.getW(key)/sx || h != NativeBinding.getH(key)/sy) && w > 0 && h > 0) {
                    if(isVerbose()) {
                        System.out.println("["+key+"]> requesting buffer resize W: " + w + ", H: " + h);
                    }
                    NativeBinding.resize(key, (int)(w*sx), (int)(h*sy));
                }

                NativeBinding.unlock(key);

                if(isVerbose()) {
                    long duration = currentTimeStamp - frameTimestamp;

                    double fps = 1e9 / duration;

                    fpsValues[fpsCounter] = fps;

                    if(fpsCounter == numValues -1) {
                        double fpsAverage = 0;
                        
                        for(double fpsVal : fpsValues) {
                            fpsAverage+=fpsVal;
                        }

                        fpsAverage/=numValues;

                        System.out.println("["+key+"]> fps: " + fpsAverage );

                        fpsCounter = 0;
                    }

                    fpsCounter++;

                    frameTimestamp = currentTimeStamp;
                } // end if verbose
                                
        };


        timer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                r.run();
            }
        };

        timer.start();

        getChildren().add(view);
    }

    /**
     * @return the verbose
     */
    public boolean isVerbose() {
        return verbose;
    }

    /**
     * @param verbose the verbose to set
     */
    public void setVerbose(boolean verbose) {
        this.verbose = verbose;
    }

    /**
     * Adds the specified listener to the native observable.
     * @param l listener to add
     */
    public void addNativeEventListener(NativeEventListener l) {
        NativeBinding.addEventListener(key, l);
    }

    /**
     * Removes the specified listener from the native observable.
     * @param l listener to remove
     */
    public void removeNativeEventListener(NativeEventListener l) {
        NativeBinding.removeEventListener(key, l);
    }

    /**
     * Disconnects this node from the connected server and removes native 
     * listeners added by this node.
     */
    public void disconnect() {
        if (key < 0) return;

        if(timer!=null) timer.stop();

        // NativeBinding.terminate(key);
        NativeBinding.removeEventListeners(key);

        getChildren().clear();

        img = null;
        view = null;
    
        buffer = null;
        intBuf = null;
    
        timer = null;
    }

    /**
     * Disconnects this node and terminates the connected server. All shared 
     * memory resources are released. Native listeners thar have been added
     * by this node will be removed as well.
     */
    public void terminate() {
        if (key < 0) return;

        if(timer!=null) timer.stop();

        NativeBinding.terminate(key);
        NativeBinding.removeEventListeners(key);

        getChildren().clear();

        img = null;
        view = null;
    
        buffer = null;
        intBuf = null;
    
        timer = null;
    }

    private void showNotConnectedText() {
        getChildren().clear();
        Label label = new Label("INFO, not connected to a server.");
        label.setStyle("-fx-text-fill: green; -fx-background-color: white; -fx-border-color: green;-fx-font-size:16");
        getChildren().add(label);
        label.layoutXProperty().bind(widthProperty().divide(2).subtract(label.widthProperty().divide(2)));
        label.layoutYProperty().bind(heightProperty().divide(2).subtract(label.heightProperty().divide(2)));    
    }

    private void showErrorText() {
        getChildren().clear();
        Label label = new Label("ERROR, cannot connect to server '"+serverName+"'.");
        label.setStyle("-fx-text-fill: red; -fx-background-color: white; -fx-border-color: red;-fx-font-size:16");
        getChildren().add(label);
        label.layoutXProperty().bind(widthProperty().divide(2).subtract(label.widthProperty().divide(2)));
        label.layoutYProperty().bind(heightProperty().divide(2).subtract(label.heightProperty().divide(2)));    
    }

    @Override
    protected double computePrefWidth(double height) {
        // TODO: consider insets ect...
        return 0;
    }

    @Override
    protected double computePrefHeight(double width) {
        // TODO: consider insets ect...
        return 0;
    }
}
