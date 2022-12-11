#include "demo_tutorial.hh"


namespace tgm
{



DemoTutorial::DemoTutorial(AudioManager & audio_manager) 
    : Tutorial("demo-tutorial", audio_manager)
{
    add_step({ "welcome", "WELCOME", 
               "Welcome to the Evolving City Generation demo.\n"
               "This is a step-by-step tutorial that will guide you through the discovery of the algorithms and the related debug tools.\n"
               "[Press the next button to go ahead]",
               "", true });
    
    // Basic controls
    add_step({ "movement", "MOVEMENT", "Move the character with the WASD keys", "", false});
    add_step({ "zoom", "ZOOM", "You can zoom in and out by the use of the mouse wheel", "", false});
    add_step({ "zoom-level", "ZOOM LEVEL", 
               "Check the current zoom level in the top-right corner of the screen."
               "[Press the next button to go ahead]",
               "tutorial_zoom_level", true});
    add_step({ "pixel-perfect", "PIXEL PERFECT RENDERING", 
               "When the zoom level is different from 0, the sprites in the game are scaled, and aren't crisp anymore.\n" 
               "So be sure that the zoom level is 1.0 in order to appreciate the artworks in their true form.\n"
               "[Press the next button to go ahead]",
               "", true});
    add_step({ "superzoom", "FASTER ZOOM", 
               "If you want to zoom faster, then you can press SHIFT while using the zoom wheel. "
               "Later this will come in handy.", 
               "", false});

    // City generation algorithm explanation
    add_step({ "first-building", "THE FIRST BUILDING", 
               "Now add the first building of the city by pressing the SPACE BAR.", 
               "", false});
    add_step({ "zoom-out-before-generation", "ZOOM OUT A BIT", 
               "Great! Now explore the just created building if you want, then, when you're ready, zoom out up to 'Zoom: 10' and beyond.", 
               "", false});
    add_step({ "city-generation", "CITY GENERATION", 
               "Now you're ready for your first city generation. Hold the key 'G'.", 
               "", false});
    add_step({ "explore-the-city-zoom-in", "CITY EXPLORATION", 
               "Now you can explore the city.\nZoom again up to 'Zoom: 1'.\n"
               "(Hold SHIFT while rolling the mouse wheel)", 
               "", false});
    add_step({ "explore-the-city", "CITY EXPLORATION", 
               "Now you can explore the city.\n"
               "With the key 'B' you can increase the character speed.\nAnd with the key 'V' you can decrease the speed.", 
               "", true});

    // City generation algorithm explanation
    add_step({ "algorithm-explanation-1", "CITY GENERATION ALGORITHM", 
               "The purpose of the algorithm is to simulate the birth and the growth of a medieval-like settlement (spontaneous architecture).",
               "tutorial_algorithm_inspiration", true});
    add_step({ "algorithm-explanation-2", "BLOCKS",
               "The algorithm is set to generate small blocks of buildings, separated by alleys.",
               "tutorial_algorithm_blocks", true});
    add_step({ "algorithm-explanation-3", "BUILDINGS",
               "You can dinsguish two buildings by the roof. A building always has a single roof, even when it's interwinded with other buildings.",
               "tutorial_algorithm_buildings", true});
    add_step({ "algorithm-explanation-3b", "DOORS",
               "Also all rooms of the same building are connected by doors (see the big building colored in red)."
               " The rooms with different colors are separated buildings, all belonging to the same block.", 
               "tutorial_algorithm_doors", true});
    add_step({ "algorithm-explanation-4", "EXPANSION", 
               "When you press 'G' the algorithm either create a new building or expand an existing one.", 
               "tutorial_algorithm_expansion", true});
    add_step({ "algorithm-explanation-5", "NEW BLOCKS", 
               "When all existing blocks have reached the maximum size, then a new building is created in a new block.", 
               "tutorial_algorithm_new_block", true});
    add_step({ "remove-building", "REMOVE A BUILDING", 
               "With BACKSPACE you can remove a building. The removed building is the one on the bottom-right tile with respect to the character.\n""Position yourself next to a building and remove it.", 
               "tutorial_remove", false});
    add_step({ "refill", "REFILL", 
               "Now if you press 'G' you see that the algorithm quickly refill the empty area that was previously occupied by the building you just removed.", 
               "", false});

    // Perspective mode
    add_step({ "roof-algorithm", "ROOF ALGORITHM", 
               "As you can see, any building has its own roof. The roof isn't just a sprite, but it's a 3D model generated by an algorithm.", 
               "", true});
    add_step({ "perspective-mode", "PERSPECTIVE MODE", 
               "Actually the whole game is set in a 3D world! It's your choice: you take the blue pill - the story ends, you wake up in your bed and believe"
               " whatever you want to believe; you press the 'P' key - you stay in Wonderland and I show you how deep the rabbit-hole goes.",
               "", false});
    add_step({ "perspective-mode-rotation", "ROTATING CAMERA IN PERSPECTIVE MODE", 
               "What a headache! You have now discovered the hard truth. The zoom still works in this mode, moreover you can rotate the camera using the arrow keys."
               " Try it yourself if you don't believe it.",
               "", false});
    add_step({ "perspective-mode-disable", "DISABLING PERSPECTIVE MODE", 
               "Go back to the orthogonal projection mode by pressing 'P'.",
               "", false});

    // Roof algorithm explanation
    add_step({ "roof-algorithm-explanation", "ROOF ALGORITHM", 
               "As you can see the roof is generated in 3D, such in a way that the final result is nice when seen from above in the orthogonal camera mode.",
               "", true});
    add_step({ "roof-algorithm-explanation-2", "ROOF ALGORITHM - MORE", 
               "The algorithm is capable of generating a hip roof starting from a tiled surface of any shape.",
               "tutorial_roof_shape", true});
    add_step({ "roof-algorithm-explanation-3", "ROOF ALGORITHM - MORE", 
               "A sinusoidal section of the roof is removed when you walk into a room. The section is obtained on the GPU side with a shader.",
               "tutorial_roof_section", true});

    // Visual Debug
    add_step({ "roof-algorithm-visual-debug", "ROOF ALGORITHM - BEHIND THE SCENE", 
               "But how does the algorithm work? And how I managed to handle the edge cases? In order to speed up the development I needed to develop"
               " a tool that allowed me to actually see each step of the algorithm.\n"
               "I called that tool 'Visual Debug' and it was really useful even for the city expansion algorithm. Let's see how it works!",
               "", true});
    add_step({ "roof-algorithm-visual-debug-depth-level", "ROOF ALGORITHM - BEHIND THE SCENE", 
               "From now on, every time a roof will be generated, all the steps of the algorithm are saved into the memory, so that later you can inspect what's happening."
               " Be aware that this operation will fill up your RAM pretty fast, since the steps are kept in memory until you exit the game.\n"
               "Admittedly the Visual Debug controls are a bit clunky, since it's not intended to be used by the player. So please be patient and follow the next steps.\n"
               "First of all you have to tell to the Visual Debugger what steps it should memorize. It could memorize every tiny step, or just the most important ones."
               " By default it memorizes no step, so you have to press the 'Y' key three times to set the depth level to 3",
               "", false});
    add_step({ "roof-algorithm-visual-debug-record", "ROOF ALGORITHM - GENERATE SOME ROOFS", 
               "Hold the key 'G' in order to generate new buildings and new roofs, so that the Visual Debug can record the algorithm steps.",
               "", false});
    add_step({ "roof-algorithm-visual-debug-activate-window", "ROOF ALGORITHM - OPEN THE VISUAL DEBUG", 
               "Press the number '3' on the keyboard (not in the numeric keypad) in order to activate the visual debug window.",
               "", false});
    add_step({ "roof-algorithm-visual-debug-record-more", "ROOF ALGORITHM - OPEN THE VISUAL DEBUG", 
               "From now on you won't see the tutorial anymore, so please memorize these controls:\n"
               "* ESC: Close the VisualDebug\n"
               "* V: Go to the previous chapter of the VisualDebug\n"
               "* M: Go to the next chapter of the VisualDebug\n"
               "* B: Go to the previous step of the VisualDebug\n"
               "* N: Go to the next step of the VisualDebug\n"
               "* H: Navigate quickly through the previous steps of the algorithm\n"
               "* J: Navigate quickly through the next steps of the algorithm\n\n"
               "When you are done, press 'G' to generate another roof and open the Visual Debug.\n",
               "", false});
    
    // Final comments
    add_step({ "end", "CONGRATULATIONS!", 
               "Congratulations! If you've got this far, you're a true nerd!", 
               "", true});
    add_step({ "whats-next", "WHAT'S NEXT?", 
               "I have a number of ideas on how to improve the algorithm:\n"
               "* First of all the generated city is just spoil. I will populate it with trees, furnitures and all sorts of decorations.\n"
               "* Secondly every building should have its own architectural style with custom walls and roofs.\n"
               "* It lacks proper roads.\n"
               "* The algorithm is really brute force and unoptimized. Also the computations should be distributed on multiple frames.\n"
               "* The list of possible improvements is just endless...\n",
               "", true});
    add_step({ "whats-next-2", "WHAT'S NEXT???", 
               "What you've seen is just the cornerstone of a game I have in mind. The time is short, but I will definitely complete it, stay tuned!",
               "", true});
}



} // namespace tgm