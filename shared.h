#include <sys/time.h>

#define  LOGI(...)  printf(__VA_ARGS__)

const float RATIO = 16.0/9.0; // twice as wide

void ident_mat(float mat[]);
void scale_mat(float mat[], float width, float height);
void trans_mat(float mat[], float x, float y);
void print_mat(float vec[]);
void mult_mat(float l[], float r[], float n[]);

GLuint loadProgram(const char *vertexShader, const char *fragmentShader);

struct {
  float fpsfactor;
} g;

static const GLint comp = 2;
static const GLsizei stride = 2;

///// TEST SHADER /////////
struct {
  GLuint id;
  GLuint MV;
  GLuint color;
  GLuint pos;
} test_shader;

void load_test_shader(int frag_offset) {
    const char vert_shad[] =
"\
attribute vec4 pos;\
uniform mat4 MV;\
varying float y;\
void main() {\
    gl_Position = MV * pos;\
    y = pos.y;\
}\
";

const char frag_shad[] =
"\
precision highp float;\
uniform vec4 col;\
varying float y;\
void main() {\
    gl_FragColor = vec4(col.r, col.g, abs(y), col.a);\
}\
";
    test_shader.id = loadProgram(vert_shad, frag_shad + frag_offset);
    test_shader.pos = glGetAttribLocation(test_shader.id, "pos");
    test_shader.MV = glGetUniformLocation(test_shader.id, "MV");
    test_shader.color = glGetUniformLocation(test_shader.id, "col");
}

void use_test_shader() {
    glVertexAttribPointer(test_shader.pos, comp, GL_FLOAT, GL_FALSE, (comp + stride)*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(test_shader.pos);
    glUseProgram(test_shader.id);
}

///// END TEST SHADER /////////

///// BACKGROUND SHADER /////////
struct {
  GLuint id;
  GLuint MV;
  GLuint color;
  GLuint pos;
} bkgnd_shader;

void load_bkgnd_shader(int frag_offset) {
    const char vert_shad[] =
"\
attribute vec4 pos;\
uniform mat4 MV;\
varying vec2 xy;\
void main() {\
    gl_Position = MV * pos;\
    xy = pos.xy;\
}\
";

const char frag_shad[] =
"\
precision highp float;\
uniform vec4 col;\
varying vec2 xy;\
void main() {\
    gl_FragColor = vec4(col.r, col.g, col.b, col.a);\
}\
";
    bkgnd_shader.id = loadProgram(vert_shad, frag_shad + frag_offset);
    bkgnd_shader.pos = glGetAttribLocation(bkgnd_shader.id, "pos");
    bkgnd_shader.MV = glGetUniformLocation(bkgnd_shader.id, "MV");
    bkgnd_shader.color = glGetUniformLocation(bkgnd_shader.id, "col");
}

void use_bkgnd_shader() {
    glVertexAttribPointer(bkgnd_shader.pos, comp, GL_FLOAT, GL_FALSE, (comp + stride)*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(bkgnd_shader.pos);
    glUseProgram(bkgnd_shader.id);
}

///// END BKGND SHADER /////////

void draw(float posx, float posy, float s_ratio) {
    use_bkgnd_shader();
    float modelview[16];
    float orthoview[16];
    float transview[16];
    ident_mat(modelview);
    float new_ratio = RATIO / s_ratio;

    if (s_ratio < RATIO)
      scale_mat(modelview, 1.0, 1.0 / new_ratio);
    else
      scale_mat(modelview, new_ratio, 1.0);

    glUniform4f(bkgnd_shader.color, 0.0, 1.0, 0.0, 1.0);
    glUniformMatrix4fv(bkgnd_shader.MV, 1, GL_FALSE, modelview);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    use_test_shader();

    ident_mat(modelview);
    glUniform4f(test_shader.color, 1.0, 1.0, 0.0, 1.0);

    float length = 0.1;
    trans_mat(modelview, sin(posx), posy + length*RATIO);
    scale_mat(modelview, length, length * RATIO);

    ident_mat(orthoview);
    if (s_ratio < RATIO)
      scale_mat(orthoview, 1.0, 1.0 / new_ratio);
    else
      scale_mat(orthoview, new_ratio, 1.0);
    mult_mat(orthoview, modelview, transview);
    glUniformMatrix4fv(test_shader.MV, 1, GL_FALSE, transview);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void update_game(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    static float xpos = -1.0;
    static float factor = 2.0;

    float s_ratio = width / (float)height;
    draw(xpos, -1, s_ratio);
    xpos += 0.01*factor * g.fpsfactor;

    static unsigned fps_accum = 0;
    static unsigned fps_counter = 0;
    static unsigned old_fps = 0;
    static unsigned fps = 0;
    static struct timeval tval_before, tval_after, diff;

    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &diff);
    gettimeofday(&tval_before, NULL);
    fps_counter++;
    fps_accum += diff.tv_usec / 1000.0f;
    if (fps_accum >= 1000) {
      old_fps = fps_counter;
      fps_accum = fps_accum - 1000;
      fps_counter = 0;
    }
    //printf("Fps %d\n", fps);
    fps = old_fps;

    glFlush();

}

static void checkGlError(const char* op) {
    char *str_error;
    for (GLenum error = glGetError(); error; error = glGetError()) {
        switch(error) {
            case GL_NO_ERROR: str_error = "GL_NO_ERROR";break;
            case GL_INVALID_ENUM: str_error = "GL_INVALID_ENUM";break;
            case GL_INVALID_VALUE: str_error = "GL_INVALID_VALUE";break;
            case GL_INVALID_OPERATION: str_error = "GL_INVALID_OPERATION";break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: str_error = "GL_INVALID_FRAMEBUFFER_OPERATION";break;
            case GL_OUT_OF_MEMORY: str_error = "GL_OUT_OF_MEMORY";break;
            default: str_error = "unknown";
        }
        NSLog(@"after %s glError %s\n", op, str_error);
    }
}

GLuint LoadShader(GLenum type, const char *shaderSrc) {
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);
    if (shader == 0) return 0;

    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char *infoLog = (char *)(malloc(sizeof(char) * (unsigned long)(infoLen)));

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            LOGI("Error compiling %s\n\n%s", infoLog, shaderSrc);
            free(infoLog);
        }
        glDeleteShader(shader);
        exit(0);
    }
    return shader;
}

void checkLinkError(GLint linked, GLuint programId, const char *vertexshader, const char *fragmentShader) {
    if(!linked) {
        GLint infoLen = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1) {
            char* infoLog = (char *)(malloc(sizeof(char) * (unsigned long)(infoLen)));
            glGetProgramInfoLog(programId, infoLen, NULL, infoLog);
            LOGI("Error linking program:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteProgram((GLuint)linked);
        LOGI("Error when linking:\n %s\n\n\n%s", vertexshader, fragmentShader);
        exit(2);
    }
}

GLuint loadProgram(const char *vertexShader, const char *fragmentShader) {
    GLuint vs, fs;
    vs = LoadShader(GL_VERTEX_SHADER, vertexShader);
    fs = LoadShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vs);
    checkGlError("Attach vertex shader");
    glAttachShader(programId, fs);
    checkGlError("Attach fragment shader");
    glLinkProgram(programId);

    GLint linked = GL_FALSE;
    glGetProgramiv(programId, GL_LINK_STATUS, &linked);
    checkLinkError(linked, programId, vertexShader, fragmentShader);
    return programId;
}

void setup(int frag_offset, int fps) {
    g.fpsfactor = 60.0 / (float) fps;
    const char *glsl_header = "";
    const char *vertex_header = glsl_header;

    const unsigned QUAD_VERTICE_SIZE = 2;
    const unsigned QUAD_TEX_SIZE = 2;
    const unsigned VERTICES = 4;
    static GLfloat vertices[] = {
        -1.0,  -1.0, // vertex coord
        0.0,  1.0, // texel coord, not used
        1.0,  -1.0,
        1.0,  1.0,
        1.0, 1.0,
        1.0,  0.0,
        -1.0, 1.0,
        0.0, 0.0
    };
    GLuint vbuffer;
    glGenBuffers(1, &vbuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * (QUAD_TEX_SIZE + QUAD_TEX_SIZE) * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    checkGlError("bind buffer");

    load_test_shader(frag_offset);
    load_bkgnd_shader(frag_offset);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ident_mat(float mat[]) {
    for (int i = 0; i < 16; i++) {
        mat[i] = 0.0;
        if ((i % 5) == 0)
            mat[i] = 1.0;
    }
}

void mult_mat(float l[], float r[], float n[]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            n[i + j*4] = l[i] * r[j*4] +
            l[i+4] * r[j*4+1] + l[i+8] * r[j*4+2] + l[i+12] * r[j*4+3];
        }
    }
}

void scale_mat(float mat[], float width, float height) {
    mat[0] *= width;
    mat[5] *= height;
}

void trans_mat(float mat[], float x, float y) {
    mat[12] += x;
    mat[13] += y;
}

void print_mat(float vec[]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f\t", vec[i + j*4]);
        }
        printf("\n");
    }
    printf("\n");
}
