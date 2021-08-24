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
package eu.mihosoft.nativefx;

import java.nio.ByteBuffer;

import animatefx.animation.FlipInY;
import javafx.application.Application;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import javafx.scene.control.MenuBar;
import javafx.scene.control.TextField;
import javafx.scene.control.ToolBar;
import javafx.scene.effect.BoxBlur;
import javafx.scene.effect.PerspectiveTransform;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.Region;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

public class NativeFXApp extends Application {

    public void start(Stage primaryStage) {

        VBox root = new VBox();

        CheckBox hidpiCB = new CheckBox("HiDPI Mode");
        CheckBox pbAPICB = new CheckBox("PixelBuffer API");
        
        TextField tf = new TextField("mem1");
        Button btn = new Button("Connect");
        Button delBtn = new Button("Delete All");
        Button effect1Btn = new Button("Blur Effect");

        effect1Btn.setOnAction((ae)-> {
            root.getChildren().filtered(n->n instanceof NativeNode).
            forEach(n->{
                BoxBlur blur = new BoxBlur(10, 10, 10);
                n.setEffect(blur);
            });
        });

        Button effectDisableBtn = new Button("Disable Effect");

        effectDisableBtn.setOnAction((ae)-> {
            root.getChildren().filtered(n->n instanceof NativeNode).
            forEach(n->{
                n.setEffect(null);
            });
        });

        Button animateBtn = new Button("Animate Nodes");

        animateBtn.setOnAction((ae)-> {
            root.getChildren().filtered(n->n instanceof NativeNode).
            forEach(n->{
                FlipInY flipInY = new FlipInY(n);
                flipInY.setSpeed(0.1);
                flipInY.setCycleCount(1);
                flipInY.play();
            });
        });

        delBtn.setOnAction((ae)-> {
            root.getChildren().filtered(n->n instanceof NativeNode).
            forEach(n->{
                NativeNode nn = (NativeNode) n;
                nn.terminate();
            });
            root.getChildren().removeIf(n->n instanceof NativeNode);
        });

        btn.setOnAction((ae)-> {

            NativeNode nativeN = new NativeNode(hidpiCB.isSelected(), pbAPICB.isSelected());

            nativeN.setVerbose(true);
            VBox.setVgrow(nativeN,Priority.SOMETIMES);
            try {
                nativeN.connect(tf.getText());
            } catch(RuntimeException ex) {
                ex.printStackTrace(System.err);
            }
            nativeN.addNativeEventListener((key, type, evt)->{
                System.out.println("> key: " + key + ", type: " + type + ", evt_msg: " + evt);
            });
            root.getChildren().add(nativeN);
        });

        ToolBar bar = new ToolBar(hidpiCB, pbAPICB, tf, btn, delBtn, effect1Btn, effectDisableBtn, animateBtn);
        root.getChildren().add(bar);

        Scene scene = new Scene(root, 1024,768);

        primaryStage.setTitle("NativeFX Demo Client (running on JFX " + System.getProperty("javafx.version")  + ")");
        primaryStage.setScene(scene);
        primaryStage.setOnCloseRequest((value)->System.exit(0));
        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }

}

