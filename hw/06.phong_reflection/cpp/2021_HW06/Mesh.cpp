#include "Mesh.h"

void Mesh::init_buffer_objects() 
{
    // TODO : initialize position, normal, index buffer
    // use your code from the previous assignment
    std::vector<float> position;
    std::vector<float> normal;

    for(int i = 0; i < mMesh->mNumVertices; i++)
    {
    position.push_back(mMesh->mVertices[i].x);
    position.push_back(mMesh->mVertices[i].y);
    position.push_back(mMesh->mVertices[i].z);

    normal.push_back(mMesh->mNormals[i].x);
    normal.push_back(mMesh->mNormals[i].y);
    normal.push_back(mMesh->mNormals[i].z);
    }

    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(float), &position[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(float), &normal[0], GL_STATIC_DRAW);

    for(int i = 0; i < mMesh->mNumFaces; i++)
    {
    Face face;
    face.num_indices = mMesh->mFaces[i].mNumIndices;

    glGenBuffers(1, &face.index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face.index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, face.num_indices * sizeof(GLuint), &mMesh->mFaces[i].mIndices[0], GL_STATIC_DRAW);

    faces.push_back(face);
    }
}

void Mesh::draw(int loc_a_position, int loc_a_normal)
{
    // TODO : draw a mesh
    // use your code from the previous assignment
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glEnableVertexAttribArray(loc_a_position);
    glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glEnableVertexAttribArray(loc_a_normal);
    glVertexAttribPointer(loc_a_normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    for (int i = 0; i < mMesh->mNumFaces; i++)
    {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faces[i].index_buffer);
    glDrawElements(GL_TRIANGLES, faces[i].num_indices, GL_UNSIGNED_INT, (void*)0);
    }

    glDisableVertexAttribArray(loc_a_position);
    glDisableVertexAttribArray(loc_a_normal);
}
    
void Mesh::print_info()
{
    std::cout << "print mesh info" << std::endl;

    std::cout << "num vertices " << mMesh->mNumVertices << std::endl;
    for (int i = 0; i < mMesh->mNumVertices; ++i)
    {
        aiVector3D vertex = mMesh->mVertices[i];
        std::cout << "  vertex  (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;

        if (mMesh->mColors[0] != NULL)
        {
            aiColor4D color = mMesh->mColors[0][i];
            std::cout << "  color  (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")" << std::endl;
        }
    }
}