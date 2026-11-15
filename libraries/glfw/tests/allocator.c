//========================================================================
// Custom heap allocator test
// Copyright (c) Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define CALL(x) (function_name = #x, x)
static const char* function_name = NULL;

struct allocator_stats
{
    size_t total;
    size_t allocations;
    size_t reallocations;
};

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void* allocate(size_t size, void* user)
{
    struct allocator_stats* stats = user;
    assert(size > 0);

    stats->total += size;
    stats->allocations++;

    printf("%s: allocate %zu bytes (allocations %zu total %zu)\n",
           function_name, size, stats->allocations, stats->total);

    void* block = malloc(size);
    assert(block != NULL);
    return block;
}

static void* reallocate(void* block, size_t size, void* user)
{
    struct allocator_stats* stats = user;
    assert(block != NULL);
    assert(size > 0);

    stats->total += size;
    stats->reallocations++;

    printf("%s: reallocate to %zu bytes (reallocations %zu total %zu)\n",
           function_name, size, stats->reallocations, stats->total);

    void* resized = realloc(block, size);
    assert(resized != NULL);
    return resized;
}

int main(void)
{
    struct allocator_stats stats = {0};
    const GLFWallocator allocator =
    {
        .allocate = allocate,
        .reallocate = reallocate,
        .user = &stats
    };

    glfwSetErrorCallback(error_callback);
    glfwInitAllocator(&allocator);

    if (!CALL(glfwInit)())
        exit(EXIT_FAILURE);

    GLFWwindow* window = CALL(glfwCreateWindow)(400, 400, "Custom allocator test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    CALL(glfwMakeContextCurrent)(window);
    gladLoadGL(glfwGetProcAddress);
    CALL(glfwSwapInterval)(1);

    while (!CALL(glfwWindowShouldClose)(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        CALL(glfwSwapBuffers)(window);
        CALL(glfwWaitEvents)();
    }

    CALL(glfwTerminate)();
    exit(EXIT_SUCCESS);
}

