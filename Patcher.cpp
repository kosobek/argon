//================================================================================================================
// Macro definitions
//================================================================================================================
// If requested, we can also use D3D disassembler to dump all info to console and HTML file.
#define DUMP_SHADER_DISASSEMBLY	1

// Additionally we can also dump raw opcodes into the output window (might be useful to reuse them when forging
// debug opcodes).
#define DUMP_RAW_OPCODES		0

// Helper macro to spare some typing
#define OPCODE_CASE(x) case x: return #x; break;





//================================================================================================================
// Include files (and optional library links).
//================================================================================================================
#include "Patcher.h"
#include <map>
#include <vector>
#include <Windows.h>

#include "DXBCChecksum.h"
#include "DXBCChecksum.cpp"
#include "d3d11TokenizedProgramFormat.hpp"
#if DUMP_SHADER_DISASSEMBLY
#include "D3DCompiler.h"

#pragma comment(lib, "D3dcompiler.lib")
#endif //DUMP_SHADER_DISASSEMBLY





//================================================================================================================
// Structures
//================================================================================================================
struct DXBCOpCode
{
	D3D10_SB_OPCODE_TYPE	opcodeType;
	unsigned int			opcodeLength;
	unsigned int			isExtended;
	DWORD*					pRawData;		//Pointer to the beginning of opcode (with opcode header)
	unsigned int			chunkOffset;	//Offset from start of the shader chunk (in bytes)
};





//================================================================================================================
// Function definitions
//================================================================================================================
const char* GetOpcodeNameString(D3D10_SB_OPCODE_TYPE opcode)
{
	switch (opcode)
	{
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_OUTPUT)
		OPCODE_CASE(D3D10_SB_OPCODE_MOV)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER)
		OPCODE_CASE(D3D10_SB_OPCODE_RET)
		OPCODE_CASE(D3D10_SB_OPCODE_ADD)
		OPCODE_CASE(D3D10_SB_OPCODE_AND)
		OPCODE_CASE(D3D10_SB_OPCODE_BREAK)
		OPCODE_CASE(D3D10_SB_OPCODE_BREAKC)
		OPCODE_CASE(D3D10_SB_OPCODE_CALL)
		OPCODE_CASE(D3D10_SB_OPCODE_CALLC)
		OPCODE_CASE(D3D10_SB_OPCODE_CASE)
		OPCODE_CASE(D3D10_SB_OPCODE_CONTINUE)
		OPCODE_CASE(D3D10_SB_OPCODE_CONTINUEC)
		OPCODE_CASE(D3D10_SB_OPCODE_CUT)
		OPCODE_CASE(D3D10_SB_OPCODE_DEFAULT)
		OPCODE_CASE(D3D10_SB_OPCODE_DERIV_RTX)
		OPCODE_CASE(D3D10_SB_OPCODE_DERIV_RTY)
		OPCODE_CASE(D3D10_SB_OPCODE_DISCARD)
		OPCODE_CASE(D3D10_SB_OPCODE_DIV)
		OPCODE_CASE(D3D10_SB_OPCODE_DP2)
		OPCODE_CASE(D3D10_SB_OPCODE_DP3)
		OPCODE_CASE(D3D10_SB_OPCODE_DP4)
		OPCODE_CASE(D3D10_SB_OPCODE_ELSE)
		OPCODE_CASE(D3D10_SB_OPCODE_EMIT)
		OPCODE_CASE(D3D10_SB_OPCODE_EMITTHENCUT)
		OPCODE_CASE(D3D10_SB_OPCODE_ENDIF)
		OPCODE_CASE(D3D10_SB_OPCODE_ENDLOOP)
		OPCODE_CASE(D3D10_SB_OPCODE_ENDSWITCH)
		OPCODE_CASE(D3D10_SB_OPCODE_EQ)
		OPCODE_CASE(D3D10_SB_OPCODE_EXP)
		OPCODE_CASE(D3D10_SB_OPCODE_FRC)
		OPCODE_CASE(D3D10_SB_OPCODE_FTOI)
		OPCODE_CASE(D3D10_SB_OPCODE_FTOU)
		OPCODE_CASE(D3D10_SB_OPCODE_GE)
		OPCODE_CASE(D3D10_SB_OPCODE_IADD)
		OPCODE_CASE(D3D10_SB_OPCODE_IF)
		OPCODE_CASE(D3D10_SB_OPCODE_IEQ)
		OPCODE_CASE(D3D10_SB_OPCODE_IGE)
		OPCODE_CASE(D3D10_SB_OPCODE_ILT)
		OPCODE_CASE(D3D10_SB_OPCODE_IMAD)
		OPCODE_CASE(D3D10_SB_OPCODE_IMAX)
		OPCODE_CASE(D3D10_SB_OPCODE_IMIN)
		OPCODE_CASE(D3D10_SB_OPCODE_IMUL)
		OPCODE_CASE(D3D10_SB_OPCODE_INE)
		OPCODE_CASE(D3D10_SB_OPCODE_INEG)
		OPCODE_CASE(D3D10_SB_OPCODE_ISHL)
		OPCODE_CASE(D3D10_SB_OPCODE_ISHR)
		OPCODE_CASE(D3D10_SB_OPCODE_ITOF)
		OPCODE_CASE(D3D10_SB_OPCODE_LABEL)
		OPCODE_CASE(D3D10_SB_OPCODE_LD)
		OPCODE_CASE(D3D10_SB_OPCODE_LD_MS)
		OPCODE_CASE(D3D10_SB_OPCODE_LOG)
		OPCODE_CASE(D3D10_SB_OPCODE_LOOP)
		OPCODE_CASE(D3D10_SB_OPCODE_LT)
		OPCODE_CASE(D3D10_SB_OPCODE_MAD)
		OPCODE_CASE(D3D10_SB_OPCODE_MIN)
		OPCODE_CASE(D3D10_SB_OPCODE_MAX)
		OPCODE_CASE(D3D10_SB_OPCODE_CUSTOMDATA)
		OPCODE_CASE(D3D10_SB_OPCODE_MOVC)
		OPCODE_CASE(D3D10_SB_OPCODE_MUL)
		OPCODE_CASE(D3D10_SB_OPCODE_NE)
		OPCODE_CASE(D3D10_SB_OPCODE_NOP)
		OPCODE_CASE(D3D10_SB_OPCODE_NOT)
		OPCODE_CASE(D3D10_SB_OPCODE_OR)
		OPCODE_CASE(D3D10_SB_OPCODE_RESINFO)
		OPCODE_CASE(D3D10_SB_OPCODE_RETC)
		OPCODE_CASE(D3D10_SB_OPCODE_ROUND_NE)
		OPCODE_CASE(D3D10_SB_OPCODE_ROUND_NI)
		OPCODE_CASE(D3D10_SB_OPCODE_ROUND_PI)
		OPCODE_CASE(D3D10_SB_OPCODE_ROUND_Z)
		OPCODE_CASE(D3D10_SB_OPCODE_RSQ)
		OPCODE_CASE(D3D10_SB_OPCODE_SAMPLE)
		OPCODE_CASE(D3D10_SB_OPCODE_SAMPLE_C)
		OPCODE_CASE(D3D10_SB_OPCODE_SAMPLE_C_LZ)
		OPCODE_CASE(D3D10_SB_OPCODE_SAMPLE_L)
		OPCODE_CASE(D3D10_SB_OPCODE_SAMPLE_D)
		OPCODE_CASE(D3D10_SB_OPCODE_SAMPLE_B)
		OPCODE_CASE(D3D10_SB_OPCODE_SQRT)
		OPCODE_CASE(D3D10_SB_OPCODE_SWITCH)
		OPCODE_CASE(D3D10_SB_OPCODE_SINCOS)
		OPCODE_CASE(D3D10_SB_OPCODE_UDIV)
		OPCODE_CASE(D3D10_SB_OPCODE_ULT)
		OPCODE_CASE(D3D10_SB_OPCODE_UGE)
		OPCODE_CASE(D3D10_SB_OPCODE_UMUL)
		OPCODE_CASE(D3D10_SB_OPCODE_UMAD)
		OPCODE_CASE(D3D10_SB_OPCODE_UMAX)
		OPCODE_CASE(D3D10_SB_OPCODE_UMIN)
		OPCODE_CASE(D3D10_SB_OPCODE_USHR)
		OPCODE_CASE(D3D10_SB_OPCODE_UTOF)
		OPCODE_CASE(D3D10_SB_OPCODE_XOR)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_RESOURCE)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_SAMPLER)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_INDEX_RANGE)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_GS_OUTPUT_PRIMITIVE_TOPOLOGY)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_GS_INPUT_PRIMITIVE)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_MAX_OUTPUT_VERTEX_COUNT)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_INPUT)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_INPUT_SGV)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_INPUT_SIV)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_INPUT_PS)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_INPUT_PS_SGV)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_INPUT_PS_SIV)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_OUTPUT_SGV)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_OUTPUT_SIV)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_TEMPS)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_INDEXABLE_TEMP)
		OPCODE_CASE(D3D10_SB_OPCODE_DCL_GLOBAL_FLAGS)
		OPCODE_CASE(D3D10_SB_OPCODE_RESERVED0)
		OPCODE_CASE(D3D10_1_SB_OPCODE_LOD)
		OPCODE_CASE(D3D10_1_SB_OPCODE_GATHER4)
		OPCODE_CASE(D3D10_1_SB_OPCODE_SAMPLE_POS)
		OPCODE_CASE(D3D10_1_SB_OPCODE_SAMPLE_INFO)
		OPCODE_CASE(D3D10_1_SB_OPCODE_RESERVED1)
		OPCODE_CASE(D3D11_SB_OPCODE_HS_DECLS)
		OPCODE_CASE(D3D11_SB_OPCODE_HS_CONTROL_POINT_PHASE)
		OPCODE_CASE(D3D11_SB_OPCODE_HS_FORK_PHASE)
		OPCODE_CASE(D3D11_SB_OPCODE_HS_JOIN_PHASE)
		OPCODE_CASE(D3D11_SB_OPCODE_EMIT_STREAM)
		OPCODE_CASE(D3D11_SB_OPCODE_CUT_STREAM)
		OPCODE_CASE(D3D11_SB_OPCODE_EMITTHENCUT_STREAM)
		OPCODE_CASE(D3D11_SB_OPCODE_INTERFACE_CALL)
		OPCODE_CASE(D3D11_SB_OPCODE_BUFINFO)
		OPCODE_CASE(D3D11_SB_OPCODE_DERIV_RTX_COARSE)
		OPCODE_CASE(D3D11_SB_OPCODE_DERIV_RTX_FINE)
		OPCODE_CASE(D3D11_SB_OPCODE_DERIV_RTY_COARSE)
		OPCODE_CASE(D3D11_SB_OPCODE_DERIV_RTY_FINE)
		OPCODE_CASE(D3D11_SB_OPCODE_GATHER4_C)
		OPCODE_CASE(D3D11_SB_OPCODE_GATHER4_PO)
		OPCODE_CASE(D3D11_SB_OPCODE_GATHER4_PO_C)
		OPCODE_CASE(D3D11_SB_OPCODE_RCP)
		OPCODE_CASE(D3D11_SB_OPCODE_F32TOF16)
		OPCODE_CASE(D3D11_SB_OPCODE_F16TOF32)
		OPCODE_CASE(D3D11_SB_OPCODE_UADDC)
		OPCODE_CASE(D3D11_SB_OPCODE_USUBB)
		OPCODE_CASE(D3D11_SB_OPCODE_COUNTBITS)
		OPCODE_CASE(D3D11_SB_OPCODE_FIRSTBIT_HI)
		OPCODE_CASE(D3D11_SB_OPCODE_FIRSTBIT_LO)
		OPCODE_CASE(D3D11_SB_OPCODE_FIRSTBIT_SHI)
		OPCODE_CASE(D3D11_SB_OPCODE_UBFE)
		OPCODE_CASE(D3D11_SB_OPCODE_IBFE)
		OPCODE_CASE(D3D11_SB_OPCODE_BFI)
		OPCODE_CASE(D3D11_SB_OPCODE_BFREV)
		OPCODE_CASE(D3D11_SB_OPCODE_SWAPC)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_STREAM)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_FUNCTION_BODY)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_FUNCTION_TABLE)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_INTERFACE)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_INPUT_CONTROL_POINT_COUNT)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_OUTPUT_CONTROL_POINT_COUNT)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_TESS_DOMAIN)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_TESS_PARTITIONING)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_TESS_OUTPUT_PRIMITIVE)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_HS_MAX_TESSFACTOR)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_HS_FORK_PHASE_INSTANCE_COUNT)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_HS_JOIN_PHASE_INSTANCE_COUNT)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_THREAD_GROUP)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_TYPED)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_RAW)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_STRUCTURED)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_RAW)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_STRUCTURED)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_RESOURCE_RAW)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_RESOURCE_STRUCTURED)
		OPCODE_CASE(D3D11_SB_OPCODE_LD_UAV_TYPED)
		OPCODE_CASE(D3D11_SB_OPCODE_STORE_UAV_TYPED)
		OPCODE_CASE(D3D11_SB_OPCODE_LD_RAW)
		OPCODE_CASE(D3D11_SB_OPCODE_STORE_RAW)
		OPCODE_CASE(D3D11_SB_OPCODE_LD_STRUCTURED)
		OPCODE_CASE(D3D11_SB_OPCODE_STORE_STRUCTURED)
		OPCODE_CASE(D3D11_SB_OPCODE_ATOMIC_AND)
		OPCODE_CASE(D3D11_SB_OPCODE_ATOMIC_OR)
		OPCODE_CASE(D3D11_SB_OPCODE_ATOMIC_XOR)
		OPCODE_CASE(D3D11_SB_OPCODE_ATOMIC_CMP_STORE)
		OPCODE_CASE(D3D11_SB_OPCODE_ATOMIC_IADD)
		OPCODE_CASE(D3D11_SB_OPCODE_ATOMIC_IMAX)
		OPCODE_CASE(D3D11_SB_OPCODE_ATOMIC_IMIN)
		OPCODE_CASE(D3D11_SB_OPCODE_ATOMIC_UMAX)
		OPCODE_CASE(D3D11_SB_OPCODE_ATOMIC_UMIN)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_ALLOC)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_CONSUME)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_IADD)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_AND)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_OR)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_XOR)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_EXCH)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_CMP_EXCH)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_IMAX)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_IMIN)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_UMAX)
		OPCODE_CASE(D3D11_SB_OPCODE_IMM_ATOMIC_UMIN)
		OPCODE_CASE(D3D11_SB_OPCODE_SYNC)
		OPCODE_CASE(D3D11_SB_OPCODE_DADD)
		OPCODE_CASE(D3D11_SB_OPCODE_DMAX)
		OPCODE_CASE(D3D11_SB_OPCODE_DMIN)
		OPCODE_CASE(D3D11_SB_OPCODE_DMUL)
		OPCODE_CASE(D3D11_SB_OPCODE_DEQ)
		OPCODE_CASE(D3D11_SB_OPCODE_DGE)
		OPCODE_CASE(D3D11_SB_OPCODE_DLT)
		OPCODE_CASE(D3D11_SB_OPCODE_DNE)
		OPCODE_CASE(D3D11_SB_OPCODE_DMOV)
		OPCODE_CASE(D3D11_SB_OPCODE_DMOVC)
		OPCODE_CASE(D3D11_SB_OPCODE_DTOF)
		OPCODE_CASE(D3D11_SB_OPCODE_FTOD)
		OPCODE_CASE(D3D11_SB_OPCODE_EVAL_SNAPPED)
		OPCODE_CASE(D3D11_SB_OPCODE_EVAL_SAMPLE_INDEX)
		OPCODE_CASE(D3D11_SB_OPCODE_EVAL_CENTROID)
		OPCODE_CASE(D3D11_SB_OPCODE_DCL_GS_INSTANCE_COUNT)

	default:
		return "UNKNOWN_OPCODE";
		break;
	}
}





void PatchDXBC(	const void		*pSrcDataShader,		//[In]	Original DXBC
				unsigned int	inSrcShaderSize,		//[In]	Size of original DXBC
				void			*pOpcodeStream,			//[In]	Opcode stream to inject
				unsigned int	inOpcodeStreamSize,		//[In]	Opcode stream size
				unsigned int	inInsertBeforeOpcode,	//[In]	Original opcode number before which modification will happen
				void			*pDstDataShader			//[Out]	Output buffer to put modified DXBC into
	)
{
#if DUMP_SHADER_DISASSEMBLY
	ID3DBlob* pBlob = NULL;

	HRESULT result = D3DDisassemble(pSrcDataShader,
									inSrcShaderSize,
									(
									D3D_DISASM_ENABLE_COLOR_CODE			|	// Enable the output of color codes.
									D3D_DISASM_ENABLE_DEFAULT_VALUE_PRINTS	|	// Enable the output of default values.
									D3D_DISASM_ENABLE_INSTRUCTION_NUMBERING |	// Enable instruction numbering.
									//D3D_DISASM_ENABLE_INSTRUCTION_CYCLE	|	// No effect.
									//D3D_DISASM_DISABLE_DEBUG_INFO			|	// Disable debug information.
									D3D_DISASM_ENABLE_INSTRUCTION_OFFSET	//|	// Enable instruction offsets.
									//D3D_DISASM_INSTRUCTION_ONLY			|	// Disassemble instructions only.
									//D3D_DISASM_PRINT_HEX_LITERALS				// Use hex symbols in disassemblies.
									),
									"// Dummy shader comment\n",
									&pBlob);

	if(result == S_OK)
	{
		// Print to debugger output window
		OutputDebugStringA((char*)pBlob->GetBufferPointer());

		// And also create to HTML file with disassembly
		FILE* pFile = fopen("shader_dump.html", "w");

		if(pFile)
		{
			fputs((char*)pBlob->GetBufferPointer(), pFile);
			fclose(pFile);
		}
		pFile = nullptr;
	}
	else
	{
		OutputDebugStringA("Error while disassembling shader");
	}
#endif //DUMP_SHADER_DISASSEMBLY



	// Shader bytecode consists of few chunks. Basic information about each chunk is
	// written in chunk index. First we will read sizes of each chunk from it.
	DWORD chunkCount = *((DWORD*)pSrcDataShader + 7);
	BYTE *pChunkIndexData = (BYTE*)pSrcDataShader + 32;

	std::vector<unsigned int> chunkIndex;

	for(unsigned int i = 0; i < chunkCount; i++)
		chunkIndex.push_back(*((DWORD*)pChunkIndexData + i));

	// Now find which one is the actual shader opcode chunk and where it begins.
	BYTE *pOpcodeChunk	= NULL;
	DWORD orgOpcodeChunkSize = 0;
	unsigned int shaderChunkIndex = 0;

	for(unsigned int i = 0; i < chunkCount; i++)
	{
		char* pCode = (char*)pSrcDataShader + chunkIndex[i];

		// Check for shader opcode chunk (most interesting for our debugging purposes are
		// SHDR (SM4) or SHEX (SM5) chunks).
		if(	(MAKEFOURCC(pCode[0], pCode[1], pCode[2], pCode[3]) == MAKEFOURCC('S', 'H', 'D', 'R')) ||
			(MAKEFOURCC(pCode[0], pCode[1], pCode[2], pCode[3]) == MAKEFOURCC('S', 'H', 'E', 'X')))
		{
			pOpcodeChunk = (BYTE*)pCode;
			shaderChunkIndex = i;
			break;
		}
	}

	// We've found shader opcode chunk
	if(pOpcodeChunk)
	{
		orgOpcodeChunkSize = *(DWORD*)(pOpcodeChunk + 4);
		unsigned char version = *(char*)(pOpcodeChunk + 8);
		unsigned char verMin = version & 0xF;
		unsigned char verMaj = version >> 4;

		// 0 = pixel
		// 1 = vertex
		unsigned char programType = *(char*)(pOpcodeChunk + 10);

		// First copy part of source DXBC buffer that remains unmodified (everything up to SHDR/SHEX shader chunk)
		unsigned int preambleSize = (unsigned int)(pOpcodeChunk - (BYTE*)pSrcDataShader);
		memcpy(pDstDataShader, pSrcDataShader, preambleSize);

		// Then copy our modified opcode chunk
		void *writeLoc = (BYTE*)pDstDataShader + preambleSize;

		// Chunk header
		memcpy(writeLoc, pOpcodeChunk, 16);
		writeLoc = (BYTE*)writeLoc + 16;

		// Then copy remainder of the shader bytecode
		unsigned int remainderByteCodeSize = (unsigned int)(inSrcShaderSize -		// Orginal length
											(pOpcodeChunk - (BYTE*)pSrcDataShader)	// size everything took up to shader chunk
											- orgOpcodeChunkSize);					// opcode chunk size

		unsigned int numberOfDWORDs = *(DWORD*)(pOpcodeChunk + 12); //should equal always orgOpcodeChunkSize / 4
		BYTE* pOpcodeData = pOpcodeChunk + 16;

		// For new shader byte what needs to be modified is:
		// 1. Whole DXBC size
		// 2. Shader chunk size
		// 3. Number of DWORDS in the chunk
		// 4. Recalculate checksum

		DWORD* opcode = (DWORD*) pOpcodeData;

		unsigned int offset = 0;
		unsigned int of = 0;

		// Collect all opcodes
		std::vector<DXBCOpCode> opcodes;

		for(unsigned int i = 0; i < numberOfDWORDs - 2; i++)
		{
			DXBCOpCode op;

			//Opcode type is stored in first 10 bits
			op.opcodeType = (D3D10_SB_OPCODE_TYPE)(opcode[i] & 1023);

			if(op.opcodeType == D3D10_SB_OPCODE_CUSTOMDATA)
			{
				OutputDebugStringA("Opcode starts custom data block. This is not supported yet!");

				// DWORD 0 (CustomDataDescTok):
				// [10:00] == D3D10_SB_OPCODE_CUSTOMDATA
				// [31:11] == D3D10_SB_CUSTOMDATA_CLASS
				//
				// DWORD 1:
				//          32-bit unsigned integer count of number
				//          of DWORDs in custom-data block,
				//          including DWORD 0 and DWORD 1.
				//          So the minimum value is 0x00000002,
				//          meaning empty custom-data.
				//
				// Layout of custom-data contents, for the various meta-data classes,
				// not defined in this file.
				//
				DWORD op1Len		= opcode[i + 1];

				op.isExtended		= false;
				op.pRawData			= opcode + i;
				op.opcodeLength		= op1Len;

				op.chunkOffset = of;

				offset	+= op1Len * 4;
				of		+= op1Len * 4;

				//Jump to next opcode
				if(op1Len > 1)
					i += op1Len - 1;
			}
			else
			{
				DWORD opControls	= (opcode[i] & 8387584) >> 11;
				DWORD op1Len		= (opcode[i] & 2139095040) >> 24;
				DWORD opIsExtended	= (opcode[i] & 2147483648) >> 31;

				op.isExtended		= opIsExtended;
				op.pRawData			= opcode + i;
				op.opcodeLength		= op1Len;
				op.chunkOffset		= of;

				offset	+= op1Len * 4;
				of		+= op1Len * 4;

				//Jump to next opcode
				if(op1Len > 1)
					i += op1Len - 1;
			}

			opcodes.push_back(op);
		}

		unsigned int insertedOpcodesSize = 0;

		// Iterate over all original opcodes and copy them into destination buffer
		for(size_t i = 0; i < opcodes.size(); i++)
		{
			// Additionally, when found the location we want to inject into, copy new opcodes there
			if(i == inInsertBeforeOpcode)
			{
				insertedOpcodesSize = inOpcodeStreamSize;
				memcpy((BYTE*)pDstDataShader + preambleSize + 16 + opcodes[i].chunkOffset, pOpcodeStream, insertedOpcodesSize);
			}

			memcpy((BYTE*)pDstDataShader + preambleSize + 16 + opcodes[i].chunkOffset + insertedOpcodesSize, opcodes[i].pRawData, opcodes[i].opcodeLength * sizeof(DWORD));

#if DUMP_RAW_OPCODES
			const char *pOpcodeName = GetOpcodeNameString(opcodes[i].opcodeType);

			std::string opcodeBuffer = "\t{ ";
			for(unsigned int j = 0; j < opcodes[i].opcodeLength; j++)
			{
				char opBuffer[32];
				sprintf(opBuffer, "%u ", opcodes[i].pRawData[j]);

				opcodeBuffer += opBuffer;
			}

			opcodeBuffer += " }";


			sprintf(buffer, "%03u. [offset: %05u, length: %02u]\t%s\t%s\n", i, opcodes[i].chunkOffset, opcodes[i].opcodeLength * sizeof(DWORD), pOpcodeName, opcodeBuffer.c_str());
			OutputDebugStringA(buffer);
#endif // DUMP_RAW_OPCODES
		}

		writeLoc = (BYTE*)writeLoc + offset + inOpcodeStreamSize;

		// Then copy remainder of the shader bytecode
		memcpy(writeLoc, pOpcodeChunk + orgOpcodeChunkSize + 8, remainderByteCodeSize - 8);


		//
		// Finally, regenerate all shader metadata in the stream
		//
		// Increment overall DXBC size by size of injected opcodes
		*((DWORD*)pDstDataShader + 6) += inOpcodeStreamSize;
		BYTE* pChunkData = (BYTE*)pDstDataShader + 32;

		// Update chunk table (offsets of all chunks after modified shader chunk)
		if(shaderChunkIndex < chunkCount) // Make sure there are remaining chunks after modified one
		{
			for(unsigned int i = shaderChunkIndex + 1; i < chunkCount; i++)
				*((DWORD*)pChunkData + i) = chunkIndex[i] + inOpcodeStreamSize;
		}

		// Update shader chunk size and number of DWORDs in it
		DWORD* pNewShaderChunk = (DWORD*)((BYTE*)pDstDataShader + preambleSize);
		pNewShaderChunk[1] += inOpcodeStreamSize;
		pNewShaderChunk[3] += (inOpcodeStreamSize / sizeof(DWORD));

		// Rehash shader MD5 checksum - checksum in buffer has to be cleared to in order to be properly hashed
		constexpr unsigned int checksumSize = 16;

		DWORD* pChecksum = (DWORD*)pDstDataShader + 1;
		memset(pChecksum, 0, checksumSize);
		unsigned int newSize = inSrcShaderSize + inOpcodeStreamSize;

		CalculateDXBCChecksum((BYTE*)pDstDataShader, newSize, pChecksum);
	}
}
