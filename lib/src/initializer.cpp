#include <GL/glew.h>
#include <initializer.hpp>
#include <constants.hpp>
#include <iostream>
#include <png.h>

uint32_t windowWidth, windowHeight;

void setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

void initGLFW(int majorVersion, int minorVersion) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* createWindow(bool fullscreen) {
    GLFWwindow* window;
    if (fullscreen) {
        GLFWmonitor* mon = glfwGetPrimaryMonitor();
        const GLFWvidmode* vidMode = glfwGetVideoMode(mon);
        window = glfwCreateWindow(vidMode->width, vidMode->height, "OpenGL", mon, NULL);
        windowWidth = vidMode->width;
        windowHeight = vidMode->height;
    }
    else {
        window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", NULL, NULL);
    }
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    //Create an OpenGL contex for the window
    glfwSwapInterval(1); //Limit framerate
    glfwMakeContextCurrent(window);
    return window;
}

void initGLAD() {
    int err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error initializing GLEW" << std::endl;
        exit(1);
    }
}

//chatgpt libpng code
uint32_t loadTexture(std::filesystem::path texturePath) {
    FILE* fp = fopen(texturePath.string().c_str(), "rb");
    if (!fp) {
        std::cerr << "[LoadPNGTexture] Failed to open file: " << texturePath << "\n";
        return -1;
    }

    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        std::cerr << "[LoadPNGTexture] Not a PNG file: " << texturePath << "\n";
        fclose(fp);
        return -1;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        std::cerr << "[LoadPNGTexture] png_create_read_struct failed\n";
        fclose(fp);
        return -1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        std::cerr << "[LoadPNGTexture] png_create_info_struct failed\n";
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        std::cerr << "[LoadPNGTexture] libpng error while reading file: " << texturePath << "\n";
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return -1;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type;
    png_get_IHDR(
        png_ptr, info_ptr,
        &width, &height,
        &bit_depth, &color_type,
        NULL, NULL, NULL
    );

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (!(color_type & PNG_COLOR_MASK_ALPHA))
        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    size_t stride = png_get_rowbytes(png_ptr, info_ptr);
    png_bytep pixels = (png_bytep)malloc(stride * height);
    if (!pixels) {
        std::cerr << "[LoadPNGTexture] Out of memory\n";
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return -1;
    }

    png_bytep* row_ptrs = (png_bytep*)malloc(sizeof(png_bytep) * height);
    if (!row_ptrs) {
        std::cerr << "[LoadPNGTexture] Out of memory (row pointers)\n";
        free(pixels);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return -1;
    }

    for (unsigned int i = 0; i < height; ++i)
        row_ptrs[height - 1 - i] = pixels + i * stride;

    png_read_image(png_ptr, row_ptrs);

    free(row_ptrs);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);

    // --- Create OpenGL texture ---
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(
        GL_TEXTURE_2D, 0,
        GL_RGBA8,
        width, height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(pixels);
    return tex;
}
