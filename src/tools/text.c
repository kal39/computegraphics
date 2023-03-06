#include "_microcanvas.h"

static char* code = //
    "#version 430\n"
    "layout(local_size_x = 1, local_size_y = 1) in;\n"
    "layout(std430, binding = 0) restrict coherent writeonly buffer ssbo0 {\n"
    "	vec4 cv[];\n"
    "};\n"
    "uniform ivec2 charData;\n"
    "uniform uvec2 cvSize;\n"
    "uniform uvec2 charPos;\n"
    "uniform vec4 charColor;\n"
    "uniform uint charScale;\n"
    "void main() {\n"
    "   uvec2 fPos = gl_GlobalInvocationID.xy / charScale;"
    "	uvec2 pPos = gl_GlobalInvocationID.xy + charPos;\n"
    "   int draw = fPos.y < 5\n"
    "       ? (charData.y >> (29 - fPos.y * 6 - fPos.x)) & 1\n"
    "       : (charData.x >> (29 - (fPos.y - 5) * 6 - fPos.x)) & 1;\n"
    "	if(bool(draw)) cv[pPos.y * cvSize.x + pPos.x] = charColor;\n"
    "}\n";

static int64_t fontBytes[] = {
    0x0000000000000000, 0x0010410404004000, 0x0000A50000000000,
    0x0000A7CA1F280000, 0x0011F51F057C4000, 0x0000065A042D3000,
    0x006249182589D000, 0x0000420000000000, 0x0018820808208180,
    0x0030208202082300, 0x000002841F10A000, 0x000001041F104000,
    0x0000000000004200, 0x000000001F000000, 0x0000000000004000,
    0x0000108408420000, 0x007924921249E000, 0x0008608202082000,
    0x0078209E1041E000, 0x0078208E0209E000, 0x0049249E02082000,
    0x0079041E0209E000, 0x0079041E1249E000, 0x0079248202082000,
    0x0079249E1249E000, 0x0079249E0209E000, 0x0000000400004000,
    0x0000000400004200, 0x0000008408102000, 0x0000001E00780000,
    0x0000020402108000, 0x0001F45107100100, 0x00FE1B692F83F000,
    0x003914511F451000, 0x0079145E1145E000, 0x003914101044E000,
    0x007914511145E000, 0x007D041E1041F000, 0x007D041E10410000,
    0x003914171144E000, 0x0045145F11451000, 0x003841040410E000,
    0x007C20820248C000, 0x0045149C12451000, 0x004104101041F000,
    0x0045B55511451000, 0x0045165513451000, 0x007D14511145F000,
    0x0079145E10410000, 0x003914511154E040, 0x0079145E14491000,
    0x003D040E0105E000, 0x007C410404104000, 0x004514511144E000,
    0x0045145111284000, 0x00451455156D1000, 0x004512840A451000,
    0x0045128404104000, 0x007C10840841F000, 0x0030820808208300,
    0x0002040804081000, 0x0030410404104300, 0x0000428000000000,
    0x0000000000000780, 0x0000810000000000, 0x000003921248D000,
    0x004107121249C000, 0x000003901040E000, 0x000823921248D000,
    0x000003121E40E000, 0x0018821C08208000, 0x000003921248E08C,
    0x0041071212492000, 0x0010010404104000, 0x0010010404104508,
    0x0041041618512000, 0x0020820808204000, 0x0000069515555000,
    0x0000071212492000, 0x000003121248C000, 0x000007121249C410,
    0x000003921248E082, 0x0000071210410000, 0x000003900C09C000,
    0x0000438404104000, 0x000004921249C000, 0x0000045111284000,
    0x000004551555A000, 0x0000044A04291000, 0x000004921248E08C,
    0x000003820420E000, 0x0018820810208180, 0x0020820808208200,
    0x0060410402104600, 0x0000000815080000, 0x0000000000000000,
};

mcv_textTool* mcv_text_tool_create() {
    mcv_textTool* textTool = malloc(sizeof(*textTool));

    uint32_t maxErrLen = 1024;
    char err[maxErrLen];

    textTool->program = mc_program_from_string(code, maxErrLen, err);
    if (textTool->program == NULL) {
        printf("%s\n", err);
        free(textTool);
        return NULL;
    }

    mcv_text_tool_set_scale(textTool, 3);
    mcv_text_tool_set_spacing(textTool, (mc_uvec2){1, 1});
    mcv_text_tool_set_color(textTool, (mc_vec4){0.8, 0.8, 0.8, 1.0});
    mcv_text_tool_set_tab_size(textTool, 4);
    return textTool;
}

mc_Result mcv_text_tool_destroy(mcv_textTool* textTool) {
    ASSERT(textTool != NULL, "textTool is NULL");
    mc_program_destroy(textTool->program);
    free(textTool);
    return OK;
}

mc_Result mcv_text_tool_set_scale(mcv_textTool* textTool, uint32_t scale) {
    textTool->scale = scale;
    mc_program_set_uint(textTool->program, "charScale", textTool->scale);
    return OK;
}

mc_Result mcv_text_tool_set_spacing(mcv_textTool* textTool, mc_uvec2 spacing) {
    textTool->spacing = spacing;
    return OK;
}

mc_Result mcv_text_tool_set_color(mcv_textTool* textTool, mc_vec4 textColor) {
    mc_program_set_vec4(textTool->program, "charColor", textColor);
    return OK;
}

mc_Result mcv_text_tool_set_tab_size(mcv_textTool* textTool, uint32_t size) {
    textTool->tabSize = 4;
    return OK;
}

mc_Result mvc_text_tool_printf(
    mcv_textTool* textTool,
    mcv_Canvas canvas,
    mc_uvec2 pos,
    char* format,
    ...
) {
    va_list args;
    va_start(args, format);
    int len = vsnprintf(NULL, 0, format, args) + 1;
    va_end(args);
    char text[len];
    va_start(args, format);
    vsnprintf(text, len, format, args);
    va_end(args);

    mc_program_set_uvec2(textTool->program, "cvSize", canvas.size);

    mc_uvec2 stride = (mc_uvec2){
        6 * textTool->scale + textTool->spacing.x,
        10 * textTool->scale + textTool->spacing.y,
    };
    mc_uvec2 off = (mc_uvec2){0, 0};
    for (uint32_t i = 0; i < strlen(text); i++) {
        char c = text[i];
        switch (text[i]) {
            case '\n': off = (mc_uvec2){0, off.y + 1}; break;
            case ' ': off.x++; break;
            case '\t': off.x += textTool->tabSize; break;
            default:
                if (c < 32) {
                    off.x++;
                    break;
                }
                mc_program_set_ivec2(
                    textTool->program,
                    "charData",
                    *(mc_ivec2*)&fontBytes[c - 32]
                );
                mc_program_set_uvec2(
                    textTool->program,
                    "charPos",
                    (mc_uvec2){
                        pos.x + off.x * stride.x,
                        pos.y + off.y * stride.y,
                    }
                );
                mc_program_dispatch(
                    textTool->program,
                    (mc_ivec3){6 * textTool->scale, 10 * textTool->scale, 1},
                    1,
                    (mc_Buffer*[]){canvas.buff}
                );
                off.x++;
                break;
        }
    }

    mc_memory_barrier();
    return OK;
}