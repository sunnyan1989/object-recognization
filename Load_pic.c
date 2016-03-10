
/* Includes ------------------------------------------------------------------*/
#include "Load_pic.h"
#include "BMP.c"
#include "BMP1.c"
#include "BMP2.c"

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/

void Draw_circle( int x0, int y0, int radius)
{
    int x = radius, y = 0;
    int xChange = 1 - (radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y) 
    {
        GLCD_PutPixel(x + x0, y + y0); // call of GCLD function;
        GLCD_PutPixel(y + x0, x + y0);
        GLCD_PutPixel(-x + x0, y + y0);
        GLCD_PutPixel(-y + x0, x + y0);
        GLCD_PutPixel(-x + x0, -y + y0);
        GLCD_PutPixel(-y + x0, -x + y0);
        GLCD_PutPixel(x + x0, -y + y0);
        GLCD_PutPixel(y + x0, -x + y0);

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

// this is similar to CRIS_draw_circle() but the circle is also filled;

void Draw_sad_face( int x0, int y0, int radius)
{
	int x = radius/2, y = 0;
    int xChange = 1 - ((radius/2) << 1);
    int yChange = 0;
    int radiusError = 0;
    // you must write your code here;
	GLCD_SetTextColor(Red);
	Draw_circle( x0, y0, radius);
	Draw_circle( x0+radius/2, y0-radius/2, radius/8);
	Draw_circle( x0-radius/2, y0-radius/2, radius/8);
	
	while (x >= y) 
    {
        GLCD_PutPixel(-x + x0, -y + y0 + 2*radius/3);
        GLCD_PutPixel(-y + x0, -x + y0 + 2*radius/3);
        GLCD_PutPixel(x + x0, -y + y0 + 2*radius/3);
        GLCD_PutPixel(y + x0, -x + y0 + 2*radius/3);

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
	GLCD_SetTextColor(White);
}

void Load_Pic(char* ptr)
{
  const unsigned char *pic;


  switch(ptr[0])
  {
	  case'A':
	    pic = BMP_DATA;
	  	break;
	  case'E':
	    pic = BMP_DATA1;
	  	break;
	  case'S':
	  	pic = BMP_DATA2;
	  	break;
	  case'U':
	    Draw_sad_face(160,120,45);
		return;
	  default:
	  	return;
  }
   GLCD_Bmp(80,40, 120, 160, (unsigned char *)pic);

}

