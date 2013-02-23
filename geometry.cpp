
void generateGrid(float* &data,unsigned int* &indices,unsigned int& vertCnt,unsigned int& triCnt,int w,int h)
{
	int pos;
	data=new float[w*h*2];
	indices=new unsigned int[w*h*3];
	pos=0;
	for (int i=0;i<h;i++)
		for (int j=0;j<w;j++)
		{
			data[pos*2]=2.0* (float)j / (float)(w-1) - 1.0;
			data[pos*2+1]=2.0* (float)i / (float)(h-1) - 1.0;
			pos++;
		}
	pos=0;
	for (int i=0;i<h-1;i++)
		for (int j=0;j<w-1;j++)
		{
			indices[pos*3]=i*h+j;
			indices[pos*3+1]=i*h+j+1;
			indices[pos*3+2]=(i+1)*h+j;

			pos++;
		}

	vertCnt = h*w;
	triCnt = (h-1)*(w-1);

}