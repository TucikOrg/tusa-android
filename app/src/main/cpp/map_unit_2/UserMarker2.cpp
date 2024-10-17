//
// Created by Artem on 17.10.2024.
//

#include "UserMarker2.h"

void UserMarker2::draw(ShadersBucket &shadersBucket, Eigen::Matrix4f pvm) {
    std::vector<float> vertices(12);
    float size = 1000;
    float height = 2 * size + size / 2;

    // bottom left
    vertices[0] = 0;
    vertices[1] = 0;
    vertices[2] = 0 + size;

    // bottom right
    vertices[3] = 0;
    vertices[4] = 0;
    vertices[5] = 0 - size;

    // top right
    vertices[6] = 0;
    vertices[7] = 0 + height;
    vertices[8] = 0 - size;

    // top left
    vertices[9] = 0;
    vertices[10] = 0 + height;
    vertices[11] = 0 + size;

    std::vector<float> uv = {
            0, 0,
            1, 0,
            1, 1.25,
            0, 1.25
    };

    auto shader = shadersBucket.userMarkerShader.get();
    glUseProgram(shader->program);
    glUniformMatrix4fv(shader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(shader->getTextureLocation(), 0);
    glVertexAttribPointer(shader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glEnableVertexAttribArray(shader->getPosLocation());
    glVertexAttribPointer(shader->getTextureCord(), 2, GL_FLOAT, GL_FALSE, 0, uv.data());
    glEnableVertexAttribArray(shader->getTextureCord());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

//    auto plainShader = shadersBucket.plainShader.get();
//    glUseProgram(plainShader->program);
//    glUniformMatrix4fv(plainShader->getMatrixLocation(), 1, GL_FALSE, pvm.data());
//    glVertexAttribPointer(plainShader->getPosLocation(), 3, GL_FLOAT, GL_FALSE, 0, vertices.data());
//    glEnableVertexAttribArray(plainShader->getPosLocation());
//    glUniform4f(plainShader->getColorLocation(), 1, 0, 0, 1);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
