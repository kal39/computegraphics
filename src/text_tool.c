#include "_microcanvas.h"

static char* code = //
    "#version 430\n"
    "layout(local_size_x = 6, local_size_y = 10) in;\n"
    "layout(std430, binding = 0) buffer ssbo0 {\n"
    "	vec4 cv[];\n"
    "};\n"
    "layout(std430, binding = 1) buffer ssbo1 {\n"
    "	int font[];\n"
    "};\n"
    "uniform uvec2 cvSize;\n"
    "uniform uint charID;\n"
    "uniform uvec2 charPos;\n"
    "uniform vec4 charColor;\n"
    "uniform uint charScale;\n"
    "void main() {\n"
    "   uvec2 fPos = gl_GlobalInvocationID.xy / charScale;"
    "	uvec2 pPos = gl_GlobalInvocationID.xy + charPos;\n"
    "   int draw = fPos.y < 5\n"
    "       ? (font[charID * 2] >> (29 - fPos.y * 6 - fPos.x)) & 1\n"
    "       : font[charID * 2 + 1] >> (29 - (fPos.y - 5) * 6 - fPos.x) & 1;\n"
    "	if(bool(draw))\n"
    "		cv[pPos.y * cvSize.x + pPos.x] = charColor;\n"
    "}\n";

static int32_t fontBytes[] = {
    0x00000000, 0x00000000, // ' '
    0x04104104, 0x00100000, // '!'
    0x00294000, 0x00000000, // '"'
    0x0000A7CA, 0x1F280000, // '#'
    0x047D47C5, 0x1F100000, // '$'
    0x00019684, 0x0B4C0000, // '%'
    0x00624625, 0x22740000, // '&'
    0x00210000, 0x00000000, // '''
    0x06208208, 0x08206000, // '('
    0x0C082082, 0x0208C000, // ')'
    0x0000A11F, 0x04280000, // '*'
    0x0000411F, 0x04100000, // '+'
    0x00000000, 0x00210000, // ','
    0x0000001F, 0x00000000, // '-'
    0x00000000, 0x00400000, // '.'
    0x00042108, 0x10800000, // '/'
    0x00792492, 0x12780000, // '0'
    0x00182082, 0x02080000, // '1'
    0x0078209E, 0x10780000, // '2'
    0x0078208E, 0x02780000, // '3'
    0x0049249E, 0x02080000, // '4'
    0x0079041E, 0x02780000, // '5'
    0x0079041E, 0x12780000, // '6'
    0x00792082, 0x02080000, // '7'
    0x0079249E, 0x12780000, // '8'
    0x0079249E, 0x02780000, // '9'
    0x00000200, 0x00200000, // ':'
    0x00000200, 0x00210000, // ';'
    0x00002108, 0x04080000, // '<'
    0x00000780, 0x1E000000, // '='
    0x00008102, 0x04200000, // '>'
    0x007D1447, 0x04004000, // '?'
    0x3F86DA6F, 0x20FC0000, // '@'
    0x0E45145F, 0x11440000, // 'A'
    0x1E451791, 0x11780000, // 'B'
    0x0E450410, 0x11380000, // 'C'
    0x1E451451, 0x11780000, // 'D'
    0x1F410790, 0x107C0000, // 'E'
    0x1F410790, 0x10400000, // 'F'
    0x0E4505D1, 0x11380000, // 'G'
    0x114517D1, 0x11440000, // 'H'
    0x0E104104, 0x04380000, // 'I'
    0x1F082082, 0x12300000, // 'J'
    0x11452712, 0x11440000, // 'K'
    0x10410410, 0x107C0000, // 'L'
    0x116D5551, 0x11440000, // 'M'
    0x11459553, 0x11440000, // 'N'
    0x1F451451, 0x117C0000, // 'O'
    0x1E451790, 0x10400000, // 'P'
    0x0E451451, 0x15381000, // 'Q'
    0x1E451794, 0x12440000, // 'R'
    0x0F410381, 0x01780000, // 'S'
    0x1F104104, 0x04100000, // 'T'
    0x11451451, 0x11380000, // 'U'
    0x11451451, 0x0A100000, // 'V'
    0x11451555, 0x1B440000, // 'W'
    0x1144A10A, 0x11440000, // 'X'
    0x1144A104, 0x04100000, // 'Y'
    0x1F042108, 0x107C0000, // 'Z'
    0x0C208208, 0x0820C000, // '['
    0x00810204, 0x02040000, // '\'
    0x0C104104, 0x0410C000, // ']'
    0x00214000, 0x00000000, // '^'
    0x00000000, 0x0001E000, // '_'
    0x00408000, 0x00000000, // '`'
    0x00000392, 0x12340000, // 'a'
    0x00410712, 0x12700000, // 'b'
    0x00000390, 0x10380000, // 'c'
    0x00082392, 0x12340000, // 'd'
    0x0000C49E, 0x10380000, // 'e'
    0x0018821C, 0x08208000, // 'f'
    0x00000392, 0x12382300, // 'g'
    0x00410712, 0x12480000, // 'h'
    0x00004004, 0x04100000, // 'i'
    0x00004004, 0x04114200, // 'j'
    0x00410598, 0x14480000, // 'k'
    0x00208208, 0x08200000, // 'l'
    0x00000695, 0x15540000, // 'm'
    0x00000712, 0x12480000, // 'n'
    0x00000312, 0x12300000, // 'o'
    0x00000312, 0x12710400, // 'p'
    0x00000312, 0x12382080, // 'q'
    0x00000712, 0x10400000, // 'r'
    0x0000E40C, 0x02700000, // 's'
    0x0010E104, 0x04100000, // 't'
    0x00000492, 0x12700000, // 'u'
    0x00000451, 0x0A100000, // 'v'
    0x00000455, 0x15680000, // 'w'
    0x00011284, 0x0A440000, // 'x'
    0x00000492, 0x12382300, // 'y'
    0x0000E084, 0x08380000, // 'z'
    0x06208210, 0x08206000, // '{'
    0x08208208, 0x08208000, // '|'
    0x18104102, 0x04118000, // '}'
    0x00000215, 0x02000000, // '~'
    0x00000000, 0x00000000, // ''
};

mcv_textTool* mcv_text_tool_create() {
    mcv_textTool* textTool = malloc(sizeof(*textTool));

    textTool->program = mc_program_from_string(code, 0, NULL);
    if (textTool->program == NULL) {
        free(textTool);
        return NULL;
    }

    textTool->fontData = mc_buffer_create(sizeof(fontBytes));
    mc_buffer_write(textTool->fontData, 0, sizeof(fontBytes), fontBytes);

    mcv_text_tool_set_scale(textTool, 3);
    mcv_text_tool_set_spacing(textTool, (mc_uvec2){1, 1});
    mcv_text_tool_set_color(textTool, (mc_vec4){0.8, 0.8, 0.8, 1.0});
    return textTool;
}

mc_Result mcv_text_tool_destroy(mcv_textTool* textTool) {
    ASSERT(textTool != NULL, "textTool is NULL");
    mc_program_destroy(textTool->program);
    mc_buffer_destroy(textTool->fontData);
    free(textTool);
    return OK;
}

mc_Result mcv_text_tool_set_scale(mcv_textTool* textTool, uint32_t scale) {
    textTool->scale = scale;
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

    mc_program_set_uint(textTool->program, "charScale", textTool->scale);
    mc_program_set_uvec2(textTool->program, "cvSize", canvas.size);

    mc_uvec2 off = (mc_uvec2){0, 0};
    for (uint32_t i = 0; i < strlen(text); i++) {
        if (text[i] == '\n') {
            off = (mc_uvec2){0, off.y + 1};
            continue;
        }

        ASSERT(32 <= text[i], "invalid character");
        mc_program_set_uint(textTool->program, "charID", text[i] - 32);

        mc_program_set_uvec2(
            textTool->program,
            "charPos",
            (mc_uvec2){
                pos.x + off.x * (6 * textTool->scale + textTool->spacing.x),
                pos.y + off.y * (10 * textTool->scale + textTool->spacing.y)}
        );

        mc_program_dispatch(
            textTool->program,
            (mc_ivec3){textTool->scale, textTool->scale, 1},
            2,
            (mc_Buffer*[]){canvas.buff, textTool->fontData}
        );

        off.x++;
    }

    return OK;
}