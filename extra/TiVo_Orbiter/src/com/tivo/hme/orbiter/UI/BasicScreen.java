package com.tivo.hme.orbiter.UI;

import java.awt.*;
import com.tivo.hme.bananas.*;

public class BasicScreen extends BScreen
{       
    /**
     * Constructor
     */
    public BasicScreen(BApplication app)
    {
        super(app);

        //
        // set the title of the screen
        //
        
        BText title = new BText(getNormal(), SAFE_TITLE_H+100, SAFE_TITLE_V, (getWidth()-(SAFE_TITLE_H*2))-100, 54);
        title.setValue(this.toString());
        title.setColor(Color.yellow);
        title.setShadow(Color.black, 3);
        title.setFlags(RSRC_VALIGN_TOP);
        title.setFont("default-48.font");
    }
}
