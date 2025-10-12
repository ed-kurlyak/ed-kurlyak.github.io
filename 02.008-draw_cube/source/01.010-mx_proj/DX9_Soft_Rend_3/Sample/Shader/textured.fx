struct VSInput
{
    float4 pos : POSITION;  // x, y, z, w (уже в screen-space)
    float2 tex : TEXCOORD;
};

struct PSInput
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD;
    float rhw : TEXCOORD1; // RHW для перспективной коррекции
};

PSInput vs_main(VSInput input)
{
    PSInput output;

    //переводим экранные координаты в диапазон шейдера -1 и +1
    output.pos.x = (2.0f * input.pos.x / 624) - 1.0f;
    output.pos.y = 1.0f - (2.0f * input.pos.y / 441);
	output.pos.z = input.pos.z;		
	output.pos.w = 1;		
    output.tex = input.tex * input.pos.w;  	
    output.rhw = input.pos.w; 

    return output;
}

sampler TextureScreen;

float4 ps_main (PSInput input) : COLOR
{
	float rhw = 1.0f / input.rhw;

	float2 tex = input.tex * rhw ;

	float4 col = tex2D(TextureScreen, tex);

	return col;

    //return float4(1.0, 0.0, 0.0, 1.0);
}

technique TexturedTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 vs_main();
		pixelShader  = compile ps_2_0 ps_main();
	}
}