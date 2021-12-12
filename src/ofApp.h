#pragma once

#define RD_WIDTH 100
#define RD_HEIGHT 66

#include "ofMain.h"

#include "ofxGui.h"
#include "ofxMidi.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp, public ofxMidiListener{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void windowResized(int w, int h);
    
        float rd_calculate_a(int x, int y);
        float rd_calculate_b(int x, int y);
        float rd_laplace_a(int x, int y);
        float rd_laplace_b(int x, int y);
        void newMidiMessage(ofxMidiMessage& eventArgs);
    
    string videos[16] = {
        "protists_passing_by.mov",
        "swimmers.mov",
        "multidao.mov",
        "multidao2.mov",
        "caicho_de_uva.mov",
        "paramecium.mov",
        "amoeba.mov",
        "rotifer.mov",
        "rotifer2.mov",
        "rotifer0.mov",
        "bolebinhas.mov",
        "mite.mov",
        "rotiver_agulinha_vortex.mov",
        "verminhos_lingueba_alga.mov",
        "vorticela_agulhas_dots.mov",
        "vorticela_agulhas_vortex.mov"
    };
    
    // GUI
    ofxPanel gui;
    ofxPanel image_panel;
    ofxPanel cv_panel;
    ofxPanel rd_panel;
    ofxPanel band_panel;
    ofxPanel hair_panel;
    bool show_ui = false;
    int menu_page = 0;
    
    // VIDEO PLAYER
    ofVideoPlayer vid_player;
    ofVideoGrabber vid_grabber;
    ofVideoGrabber cameras[5];
    ofxToggle use_webcam;
    ofxIntSlider current_video;
    ofxIntSlider current_camera;
    
    // IMAGE
    ofxIntSlider image_r;
    ofxIntSlider image_g;
    ofxIntSlider image_b;
    ofxIntSlider image_a;
    
    // OPENCV
    ofxCvColorImage cv_color_img;
    ofxCvGrayscaleImage cv_gray_image;
    ofxCvGrayscaleImage cv_gray_bg;
    ofxCvGrayscaleImage cv_gray_diff;
    ofxCvContourFinder cv_contour_finder;
    bool cv_learn_background;
    
    ofxFloatSlider cv_threshold;
    ofxFloatSlider cv_influence;
    
    ofxIntSlider cv_r;
    ofxIntSlider cv_g;
    ofxIntSlider cv_b;
    ofxIntSlider cv_a;
    ofxToggle cv_rect;
    ofxToggle cv_blob;
    
    // REACTION DIFFUSION
    float rd_grid[RD_WIDTH][RD_HEIGHT][2];
    float rd_next[RD_WIDTH][RD_HEIGHT][2];
    int rd_width = RD_WIDTH;
    int rd_height = RD_HEIGHT;
    
    ofxFloatSlider rd_dt;
    ofxFloatSlider rd_da;
    ofxFloatSlider rd_db;
    ofxFloatSlider rd_feed;
    ofxFloatSlider rd_k;
    ofxFloatSlider rd_threshold;
    ofxFloatSlider rd_cell_size;
    
    ofxIntSlider rd_r;
    ofxIntSlider rd_g;
    ofxIntSlider rd_b;
    ofxIntSlider rd_a;
    
    // Bands around blobs
    ofxIntSlider band_r;
    ofxIntSlider band_g;
    ofxIntSlider band_b;
    ofxIntSlider band_a;
    ofxIntSlider band_n;
    ofxIntSlider band_size;
    
    // Hairy blobs
    ofxIntSlider hair_r;
    ofxIntSlider hair_g;
    ofxIntSlider hair_b;
    ofxIntSlider hair_a;
    ofxIntSlider hair_dist;
    ofxIntSlider hair_size;
    ofxToggle hair_spiky;
    ofxToggle hair_pentelho;
    ofxToggle hair_undulating;
    
    
    // MIDI IN
    ofxMidiIn midiIn;
    ofxMidiOut midiOut;
    std::vector<ofxMidiMessage> midiMessages;
    std::size_t maxMessages = 10; //< max number of messages to keep track of
};
