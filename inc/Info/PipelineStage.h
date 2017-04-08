#ifndef __VKPP_INFO_PIPELINE_STAGE_H__
#define __VKPP_INFO_PIPELINE_STAGE_H__



#include <Info/Common.h>
#include <Info/Flags.h>



namespace vkpp
{



enum class PipelineStageFlagBits
{
    eTopOfPipe                      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    eDrawIndirect                   = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
    eVertexInput                    = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
    eVertexShader                   = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
    eTessellationControlShader      = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT,
    eTessellationEvaluationShader   = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT,
    eGeometryShader                 = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT,
    eFragmentShader                 = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    eEarlyFragmentTests             = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
    eLateFragementTests             = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
    eColorAttachmentOutput          = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    eComputeShader                  = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    eTransfer                       = VK_PIPELINE_STAGE_TRANSFER_BIT,
    eBottomOfPipe                   = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    eHost                           = VK_PIPELINE_STAGE_HOST_BIT,
    eAllGraphics                    = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    eAllCommands                    = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
    eCommandProcessNVX              = VK_PIPELINE_STAGE_COMMAND_PROCESS_BIT_NVX
};

using PipelineStageFlags = internal::Flags<PipelineStageFlagBits, VkPipelineStageFlags>;



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_PIPELINE_STAGE_H__
