package com.libertyrecomp;

import org.libsdl.app.SDLActivity;

public class LibertyActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL3",
            "LibertyUniversalCore"
        };
    }
}
