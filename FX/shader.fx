// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     shader.fx
//   Author:
//     Floris van Riel
//   Description:
//     Contains the main shaders for each
//     scene and mesh type
//
// An AP product (c) 2016
// ------------------------------------ //

// Constant buffers
cbuffer cbPerObject {
	float4x4 g_worldViewProj;
};

struct VertexIn {
	float3 Pos : POSITION;
	float4 Color : COLOR;
};

struct VertexOut {
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};

RasterizerState SolidRS {
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

VertexOut VS(VertexIn vin) {
	VertexOut vout;

	// Apply transformation matrices to the vertex
	vout.PosH = mul(float4(vin.Pos, 1.0f), g_worldViewProj);

	// Keep the same color
	vout.Color = vin.Color;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target {
	// A simple pass through shader
	return pin.Color;
}

technique11 ColorTech {
	pass P0 {
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));

		SetRasterizerState(SolidRS);
	}
}