#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // GUI
    image_panel.setup("IMG", "image_settings.xml", 0, 0);
    image_panel.add(use_webcam.setup("use webcam", false));
    image_panel.add(current_video.setup("video", 0, 0, 15));
    image_panel.add(current_camera.setup("cam", 0, 0, 4));
    image_panel.add(image_r.setup("r", 0, 0, 255));
    image_panel.add(image_g.setup("g", 255, 0, 255));
    image_panel.add(image_b.setup("b", 100, 0, 255));
    image_panel.add(image_a.setup("a", 0, 0, 255));
    
    cv_panel.setup("CV", "cv_settings.xml", 0, 160);
    cv_panel.add(cv_threshold.setup("threshold", 50, 1.0, 100.0));
    cv_panel.add(cv_r.setup("r", 255, 0, 255));
    cv_panel.add(cv_g.setup("g", 0, 0, 255));
    cv_panel.add(cv_b.setup("b", 100, 0, 255));
    cv_panel.add(cv_a.setup("a", 0, 0, 255));
    
    rd_panel.setup("RD", "rd_settings.xml", 0, 300);
    rd_panel.add(rd_dt.setup("dt", 1.0, 0.0, 2.0));
    rd_panel.add(rd_da.setup("dA", 1.0, 0.0, 1.0));
    rd_panel.add(rd_db.setup("dB", 0.5, 0.0, 1.0));
    rd_panel.add(rd_feed.setup("feed", 0.055, 0.0, 0.1));
    rd_panel.add(rd_k.setup("kill", 0.062, 0.0, 0.1));
    rd_panel.add(rd_threshold.setup("threshold", 0.5, 0.0, 1.0));
    rd_panel.add(cv_influence.setup("cv_influence", 0.5, 0.0, 1.0));
    rd_panel.add(rd_r.setup("r", 255, 0, 255));
    rd_panel.add(rd_g.setup("g", 255, 0, 255));
    rd_panel.add(rd_b.setup("b", 255, 0, 255));
    rd_panel.add(rd_a.setup("a", 0, 0, 255));
    rd_panel.add(rd_cell_size.setup("size", 0.1, 0.0, 1.0));
    
    
    // VIDEO PLAYER
    vid_player.load(videos[current_video]);
    vid_player.setVolume(0.0);
    
    // WEBCAMS
//    for (int i = 0; i < 3; i++) {
//        cameras[i].setDeviceID(i);
//        cameras[i].setup(cameras[i].getWidth(),cameras[i].getHeight());
//        cameras[i].initGrabber(cameras[i].getWidth(),cameras[i].getHeight());
//    }

    // CV
//    int w = cameras[current_camera].getWidth() * 2;
//    int h = cameras[current_camera].getHeight() * 2;
    int w = vid_player.getWidth();
    int h = vid_player.getHeight();
    cv_color_img.allocate(w,h);
    cv_gray_image.allocate(w,h);
    cv_gray_bg.allocate(w,h);
    cv_gray_diff.allocate(w,h);
    
    // MIDI
//    midiIn.listInPorts();
//    midiIn.openPort("IAC Driver Bus 1");
//    midiIn.addListener(this);
//
//    midiOut.listOutPorts();
//    midiOut.openPort("IAC Driver Bus 1");
}

//--------------------------------------------------------------
void ofApp::update() {
    // Update current video
    string path = vid_player.getMoviePath();
    if (path != videos[current_video]) {
        vid_player.load(videos[current_video]);
    }
    
    // openCV update images
    vid_player.update();
//    ofVideoGrabber grabber = cameras[current_camera];
//    grabber.update();
    
//    cv_color_img.setFromPixels(grabber.getPixels());
    cv_color_img.setFromPixels(vid_player.getPixels());
    cv_gray_image = cv_color_img;
    cv_gray_diff.absDiff(cv_gray_bg, cv_gray_image);
    cv_gray_diff.threshold(cv_threshold);
    
    // openCV blob detection
    cv_contour_finder.findContours(cv_gray_diff, 1, (ofGetWidth()*ofGetHeight())/9, 1000, true);
    
    // openCV data to Reaction Diffusion grid
    for (int i = 0; i < cv_contour_finder.nBlobs; i++){
        ofRectangle r = cv_contour_finder.blobs.at(i).boundingRect;
        float x = ofMap(r.x, 0, cv_contour_finder.getWidth(), 0, ofGetWidth());
        float y = ofMap(r.y, 0, cv_contour_finder.getHeight(), 0, ofGetHeight());
        int gx = ofMap(x, 0, ofGetWindowWidth()-1.0, 0, rd_width);
        int gy = ofMap(y, 0, ofGetWindowHeight()-1.0, 0, rd_height);
        rd_grid[gx][gy][1] += cv_influence;
        rd_grid[gx][gy][1] = CLAMP(rd_grid[gx][gy][1], 0.0, 1.0);
    }
    
    // Reaction diffusion
    for (int i = 0; i < 10; i++) {
        for (int x = 1; x < rd_width-1; x++) {
            for (int y = 1; y < rd_height-1; y++) {
                float a = rd_calculate_a(x, y);
                float b = rd_calculate_b(x, y);
                rd_next[x][y][0] = CLAMP(a, 0.0, 1.0);
                rd_next[x][y][1] = CLAMP(b, 0.0, 1.0);
            }
        }
        swap(rd_grid, rd_next);
    }
    
    // Send midi data
    float average_x = 0;
    float average_y = 0;
    float average_w = 0;
    float average_h = 0;
    for (int i = 0; i < cv_contour_finder.nBlobs; i++){
        ofRectangle r = cv_contour_finder.blobs.at(i).boundingRect;
        float x = ofMap(r.x, 0, cv_contour_finder.getWidth(), 0, ofGetWidth());
        float y = ofMap(r.y, 0, cv_contour_finder.getHeight(), 0, ofGetHeight());
        float w = ofMap(r.width, 0, cv_contour_finder.getWidth(), 0, ofGetWidth());
        float h = ofMap(r.height, 0, cv_contour_finder.getHeight(), 0, ofGetHeight());
        average_x += x;
        average_y += y;
        average_w += w;
        average_h += h;
    }
    
    average_x /= cv_contour_finder.nBlobs;
    average_y /= cv_contour_finder.nBlobs;
    average_w /= cv_contour_finder.nBlobs;
    average_h /= cv_contour_finder.nBlobs;
    
    midiOut.sendControlChange(2, 0, average_x);
    midiOut.sendControlChange(2, 1, average_y);
    midiOut.sendControlChange(2, 2, average_w);
    midiOut.sendControlChange(2, 3, average_h);
    midiOut.sendControlChange(2, 4, cv_contour_finder.nBlobs);
}

//--------------------------------------------------------------
void ofApp::draw() {
    // openCV color image
    ofPushStyle();
    cv_color_img.draw(0,0,ofGetWidth(),ofGetHeight());
    ofPopStyle();
    
    // openCV grayscale image colorized
    ofPushStyle();
    ofSetColor(image_r, image_g, image_b, image_a);
    cv_gray_image.draw(0,0,ofGetWidth(),ofGetHeight());
    ofPopStyle();

    // openCV blob rectangles
    ofPushStyle();
    ofFill();
    for (int i = 0; i < cv_contour_finder.nBlobs; i++){
        // Draw openCV blobs
        ofRectangle r = cv_contour_finder.blobs.at(i).boundingRect;
        ofSetColor(cv_r, cv_g, cv_b, cv_a);
        float x = ofMap(r.x, 0, cv_contour_finder.getWidth(), 0, ofGetWidth());
        float y = ofMap(r.y, 0, cv_contour_finder.getHeight(), 0, ofGetHeight());
        float w = ofMap(r.width, 0, cv_contour_finder.getWidth(), 0, ofGetWidth());
        float h = ofMap(r.height, 0, cv_contour_finder.getHeight(), 0, ofGetHeight());
        ofDrawRectangle(x, y, w, h);
    }
    ofPopStyle();

    // Reaction diffusion grid
    ofPushStyle();
    ofVec2f res = ofVec2f(ofGetWindowWidth()/rd_width, ofGetWindowHeight()/rd_height);
    for (int x = 0; x < rd_width; x++) {
        for (int y = 0; y < rd_height; y++) {
            float a = rd_grid[x][y][0];
            float b = rd_grid[x][y][1];
            float c = 1.0-CLAMP(a-b, 0.0, 1.0);
            if (c > rd_threshold) {
                ofSetColor(rd_r, rd_g, rd_b, rd_a);
                ofDrawRectangle(
                    4*res.x + x*res.x,
                    4*res.y + y*res.y,
                    res.x * rd_cell_size,
                    res.y * rd_cell_size
                    );
            } else {
                ofSetColor(0, 0, 0, rd_a);
            }
            

        }
    }
    ofPopStyle();
    
    // GUI
    if (show_ui) {
        image_panel.draw();
        cv_panel.draw();
        rd_panel.draw();
    }
    
//    if (midiMessages.size() > 0) {
//        ofxMidiMessage &msg = midiMessages[midiMessages.size()-1];
//        if(msg.status == MIDI_CONTROL_CHANGE && msg.channel == 1) {
//            switch(msg.control) {
//                case 0:
//                    image_r = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 1:
//                    image_g = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 2:
//                    image_b = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 3:
//                    image_a = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 4:
//                    cv_r = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 5:
//                    cv_g = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 6:
//                    cv_b = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 7:
//                    cv_a = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 8:
//                    cv_threshold = ofMap(msg.value, 0, 127, 0, 100);
//                    break;
//                case 9:
//                    rd_r = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 10:
//                    rd_g = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 11:
//                    rd_b = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 12:
//                    rd_a = ofMap(msg.value, 0, 127, 0, 255);
//                    break;
//                case 13:
//                    rd_k = ofMap(msg.value, 0, 127, 0, 0.1);
//                    break;
//                case 14:
//                    rd_da = ofMap(msg.value, 0, 127, 0, 1);
//                    break;
//                case 15:
//                    rd_db = ofMap(msg.value, 0, 127, 0, 1);
//                    break;
//                case 16:
//                    rd_feed = ofMap(msg.value, 0, 127, 0, 0.1);
//                    break;
//                case 17:
//                    cv_influence = ofMap(msg.value, 0, 127, 0, 1);
//                    break;
//                case 18:
//                    rd_cell_size = ofMap(msg.value, 0, 127, 0, 1);
//                    break;
//            }
//        }
//    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
//    int w = vid_player.getWidth();
//    int h = vid_player.getHeight();
    switch (key) {
        case ' ':
            show_ui = !show_ui;
            break;
        case '1':
//            cv_gray_bg.set(0);
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
    
    // add the latest message to the message queue
//    midiMessages.push_back(msg);
}

//--------------------------------------------------------------

float ofApp::rd_laplace_a(int x, int y) {
    float sumA = 0;
    sumA += rd_grid[x][y][0] * -1.0;
    sumA += rd_grid[x - 1][y][0] * 0.2;
    sumA += rd_grid[x + 1][y][0] * 0.2;
    sumA += rd_grid[x][y + 1][0] * 0.2;
    sumA += rd_grid[x][y - 1][0] * 0.2;
    sumA += rd_grid[x - 1][y - 1][0] * 0.05;
    sumA += rd_grid[x + 1][y - 1][0] * 0.05;
    sumA += rd_grid[x + 1][y + 1][0] * 0.05;
    sumA += rd_grid[x - 1][y + 1][0] * 0.05;
    return sumA;
}

float ofApp::rd_laplace_b(int x, int y) {
    float sumB = 0;
    sumB += rd_grid[x][y][1] * -1.0;
    sumB += rd_grid[x - 1][y][1] * 0.2;
    sumB += rd_grid[x + 1][y][1] * 0.2;
    sumB += rd_grid[x][y + 1][1] * 0.2;
    sumB += rd_grid[x][y - 1][1] * 0.2;
    sumB += rd_grid[x - 1][y - 1][1] * 0.05;
    sumB += rd_grid[x + 1][y - 1][1] * 0.05;
    sumB += rd_grid[x + 1][y + 1][1] * 0.05;
    sumB += rd_grid[x - 1][y + 1][1] * 0.05;
    return sumB;
}

float ofApp::rd_calculate_a(int x, int y) {
    float a = rd_grid[x][y][0];
    float b = rd_grid[x][y][1];
    return a + (rd_da * rd_laplace_a(x, y) - a * b * b + rd_feed * (1 - a))*rd_dt;
}
float ofApp::rd_calculate_b(int x, int y) {
    float a = rd_grid[x][y][0];
    float b = rd_grid[x][y][1];
    return b + (rd_db * rd_laplace_b(x, y) + a * b * b - (rd_k + rd_feed) * b)*rd_dt;
}
