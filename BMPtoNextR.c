#include <stdio.h>

unsigned short header[0x36/2];
unsigned char palette[0x400];
unsigned char newpal[0x400];
unsigned char image[8192*8192];
unsigned char rimage[8192*8192];
unsigned char fimage[8192*8192];

unsigned char palindex[256];

float r,g,b;
int i,o,j,k,l,ri,gi,bi,loc;

FILE *f;
char fn[512];

int main(int c, char **s)
{
	if(c<2)
	{
		printf("BMPtoNext Usage :-\nBMPtoNext srcfile.bmp <dstfile.bmp>\n");
		return 0;
	}
	sprintf(fn,"%s",s[1]);

	f=fopen(fn,"rb");
	if(f==NULL)
	{
		printf("Can't open %s\n",s[1]);
		exit(1);
	}
	fread(header,0x36,1,f);
	fread(palette,256,4,f);
	fread(image,header[9],header[11],f);
	fclose(f);
	
	for(i=0;i<256;i++)
	{
		r=palette[i*4+2];				//bmp palette is ordered B/G/R/A * 256 after the 0x36 byte header
		g=palette[i*4+1];
		b=palette[i*4+0];
		r=r+((255.0f/7.0f)/2.0f);	//add half a unit to round up colours to closest unit
		ri=r/(255.0f/7.0f);			//get an int from 0 to 7 for Red
		g=g+((255.0f/7.0f)/2.0f);
		gi=g/(255.0f/7.0f);			//get an int from 0 to 7 for Green
		b=b+((255.0f/3.0f)/2.0f);
		bi=b/(255.0f/3.0f);			//get an int from 0 to 3 for Blue
		palindex[i]=((ri&7)<<5)+((gi&7)<<2)+(bi&3);	// palette index for this pixel value in the bmp is the RRRGGGBB index for Next
		newpal[i*4+3]=0;
		newpal[i*4+2]=(unsigned char)((i>>5)&7)*(255.0f/7.0f);
		newpal[i*4+1]=(unsigned char)((i>>2)&7)*(255.0f/7.0f);
		newpal[i*4+0]=(unsigned char)((i>>0)&3)*(255.0f/3.0f);
	}
	for(i=0;i<header[9]*header[11];i++)		// go through each pixel in the image ( screen width * screen height )
	{
		image[i]=palindex[image[i]];			// and change it to it's new index from the Next palette.
	}

	if(c==2)	sprintf(fn,"%s",s[1]);		// if only 1 filename write over original
	else		sprintf(fn,"%s",s[2]);		// otherwise use new filename for destination
	f=fopen(fn,"wb");						// create file
	if(f==NULL)
	{
		printf("Can't create %s\n",fn);
		exit(1);
	}
	


	//rewrite image
	int max = (256*192);
	int old = max - 1;

	for(i=0;i<max;i++)
	{

		rimage[i] = image[old];
		old--;

	}

	int rows = 192;
	int cols = 256;
	//flip image
	for(o=0;o<rows;o++)
	{
		loc = (o * 256);	
		for(i=0;i<cols;i++)
		{

		fimage [loc + 255 - i] = rimage[loc + i];

		}


	}

		fimage [max-1] = rimage[max - 255];


	
	fwrite(header,0x36,1,f);						// save header
	fwrite(newpal,256,4,f);							// save palette
	fwrite(fimage,header[9],header[11],f);	// save image
	fclose(f);											// close file

	return 0;											// return with no error.
}