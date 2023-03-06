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
    "       ? (charData.x >> (29 - fPos.y * 6 - fPos.x)) & 1\n"
    "       : (charData.y >> (29 - (fPos.y - 5) * 6 - fPos.x)) & 1;\n"
    "	if(bool(draw)) cv[pPos.y * cvSize.x + pPos.x] = charColor;\n"
    "}\n";

static int32_t fontData[] = {
    0x00000000, 0x00000000, 0x00104104, 0x04004000, 0x0000A500, 0x00000000,
    0x0000A7CA, 0x1F280000, 0x0011F51F, 0x057C4000, 0x0000065A, 0x042D3000,
    0x00624918, 0x2589D000, 0x00004200, 0x00000000, 0x00188208, 0x08208180,
    0x00302082, 0x02082300, 0x00000284, 0x1F10A000, 0x00000104, 0x1F104000,
    0x00000000, 0x00004200, 0x00000000, 0x1F000000, 0x00000000, 0x00004000,
    0x00001084, 0x08420000, 0x00792492, 0x1249E000, 0x00086082, 0x02082000,
    0x0078209E, 0x1041E000, 0x0078208E, 0x0209E000, 0x0049249E, 0x02082000,
    0x0079041E, 0x0209E000, 0x0079041E, 0x1249E000, 0x00792482, 0x02082000,
    0x0079249E, 0x1249E000, 0x0079249E, 0x0209E000, 0x00000004, 0x00004000,
    0x00000004, 0x00004200, 0x00000084, 0x08102000, 0x0000001E, 0x00780000,
    0x00000204, 0x02108000, 0x0001F451, 0x07100100, 0x00FE1B69, 0x2F83F000,
    0x00391451, 0x1F451000, 0x0079145E, 0x1145E000, 0x00391410, 0x1044E000,
    0x00791451, 0x1145E000, 0x007D041E, 0x1041F000, 0x007D041E, 0x10410000,
    0x00391417, 0x1144E000, 0x0045145F, 0x11451000, 0x00384104, 0x0410E000,
    0x007C2082, 0x0248C000, 0x0045149C, 0x12451000, 0x00410410, 0x1041F000,
    0x0045B555, 0x11451000, 0x00451655, 0x13451000, 0x007D1451, 0x1145F000,
    0x0079145E, 0x10410000, 0x00391451, 0x1154E040, 0x0079145E, 0x14491000,
    0x003D040E, 0x0105E000, 0x007C4104, 0x04104000, 0x00451451, 0x1144E000,
    0x00451451, 0x11284000, 0x00451455, 0x156D1000, 0x00451284, 0x0A451000,
    0x00451284, 0x04104000, 0x007C1084, 0x0841F000, 0x00308208, 0x08208300,
    0x00020408, 0x04081000, 0x00304104, 0x04104300, 0x00004280, 0x00000000,
    0x00000000, 0x00000780, 0x00008100, 0x00000000, 0x00000392, 0x1248D000,
    0x00410712, 0x1249C000, 0x00000390, 0x1040E000, 0x00082392, 0x1248D000,
    0x00000312, 0x1E40E000, 0x0018821C, 0x08208000, 0x00000392, 0x1248E08C,
    0x00410712, 0x12492000, 0x00100104, 0x04104000, 0x00100104, 0x04104508,
    0x00410416, 0x18512000, 0x00208208, 0x08204000, 0x00000695, 0x15555000,
    0x00000712, 0x12492000, 0x00000312, 0x1248C000, 0x00000712, 0x1249C410,
    0x00000392, 0x1248E082, 0x00000712, 0x10410000, 0x00000390, 0x0C09C000,
    0x00004384, 0x04104000, 0x00000492, 0x1249C000, 0x00000451, 0x11284000,
    0x00000455, 0x1555A000, 0x0000044A, 0x04291000, 0x00000492, 0x1248E08C,
    0x00000382, 0x0420E000, 0x00188208, 0x10208180, 0x00208208, 0x08208200,
    0x00604104, 0x02104600, 0x00000008, 0x15080000, 0x00000000, 0x00000000,
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
                    (mc_ivec2){
                        fontData[2 * (c - 32)],
                        fontData[2 * (c - 32) + 1],
                    }
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