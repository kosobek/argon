// Author: Henryk Kosobucki, 2016

//=============================================	OVERVIEW =========================================================
//	DXBC (D3D assembly) patcher utility. It's main purpose is to patch DXBC bytecode (useful for scenarios when we
//	don't have HLSL, fxc compiler generates incorrect bytecode or simply "educational" purposes like replacing
//	shaders in your favourite game :)).
//
//	Version 0.1 - basic patch ability for 'SHEX' and 'SHDR' bytecode chunks (opcode chunks).
//================================================================================================================

#ifndef PATCH_DXBC_H
#define PATCH_DXBC_H

void PatchDXBC(	const void		*pSrcDataShader,		//[In]	Original DXBC
				unsigned int	inSrcShaderSize,		//[In]	Size of original DXBC
				void			*pOpcodeStream,			//[In]	Opcode stream to inject
				unsigned int	inOpcodeStreamSize,		//[In]	Opcode stream size
				unsigned int	inInsertBeforeOpcode,	//[In]	Original opcode number before which modification will happen
				void			*pDstDataShader			//[Out]	Output buffer to put modified DXBC into
	);

#endif // PATCH_DXBC_H
