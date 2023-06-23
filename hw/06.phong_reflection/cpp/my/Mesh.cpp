// Mesh.cpp
#include "Mesh.h"

void Mesh::gen_gl_buffers()
{
    glGenBuffers(1, &position_buffer_);
    glGenBuffers(1, &color_buffer_);
    glGenBuffers(1, &normal_buffer_);
}

void Mesh::update_tv_indices()
{
    // triangle-vertex indices
    tv_indices_.clear();
    for (unsigned int i = 0; i < pmesh_->mNumFaces; ++i)
    {
        aiFace &ai_face = pmesh_->mFaces[i];
        assert(ai_face.mNumIndices >= 3);

        // convert a polygon to a triangle fan
        for (unsigned int idx = 0; idx < ai_face.mNumIndices - 2; ++idx)
        {
            tv_indices_.push_back(ai_face.mIndices[0]);
            tv_indices_.push_back(ai_face.mIndices[idx + 1]);
            tv_indices_.push_back(ai_face.mIndices[idx + 2]);
        }
    }
}

void Mesh::set_gl_position_buffer_()
{
    assert(pmesh_->HasPositions());

    std::vector<glm::vec3> tv_positions; // per triangle-vertex 3D position (size = 3 x #triangles)

    // TODO: for each triangle, set tv_positions
    for (unsigned int i = 0; i < tv_indices_.size(); ++i)
    {
        unsigned int vertexIndex = tv_indices_[i];
        aiVector3D vertex = pmesh_->mVertices[vertexIndex];
        tv_positions.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));
    }

    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_positions.size(), &tv_positions[0], GL_STATIC_DRAW);
}

void Mesh::set_gl_color_buffer_(unsigned int cs_idx)
{
    assert(pmesh_->HasVertexColors(cs_idx));

    std::vector<glm::vec3> tv_colors; // per triangle-vertex 3D position (size = 3 x #triangles)

    // TODO: for each triangle, set tv_colors
    for (unsigned int i = 0; i < tv_indices_.size(); ++i)
    {
        unsigned int vertexIndex = tv_indices_[i];
        aiColor4D color = pmesh_->mColors[cs_idx][vertexIndex];
        tv_colors.push_back(glm::vec3(color.r, color.g, color.b));
    }

    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_colors.size(), &tv_colors[0], GL_STATIC_DRAW);

    is_color_ = true;
}

void Mesh::set_gl_normal_buffer_(ShadingType shading_type)
{
    std::vector<glm::vec3> tv_flat_normals;   // per triangle-vertex flat normal (size = 3 x #triangles)
    std::vector<glm::vec3> tv_smooth_normals; // per triangle-vertex smooth normal (size = 3 x #triangles)
    std::vector<glm::vec3> v_smooth_normals;  // per-vertex 3D normal (size = #vertices)

    // init normals
    v_smooth_normals.resize(pmesh_->mNumVertices);
    for (std::size_t i = 0; i < v_smooth_normals.size(); ++i)
        v_smooth_normals[i] = glm::vec3(0.0f, 0.0f, 0.0f);

    // Compute per-triangle normals & add up to tv_flat_normals & v_smooth_normals
    tv_flat_normals.resize(tv_indices_.size());
    for (unsigned int i = 0; i < tv_indices_.size(); i += 3)
    {
        unsigned int v0 = tv_indices_[i];
        unsigned int v1 = tv_indices_[i + 1];
        unsigned int v2 = tv_indices_[i + 2];

        aiVector3D vertex0 = pmesh_->mVertices[v0];
        aiVector3D vertex1 = pmesh_->mVertices[v1];
        aiVector3D vertex2 = pmesh_->mVertices[v2];

        glm::vec3 position0(vertex0.x, vertex0.y, vertex0.z);
        glm::vec3 position1(vertex1.x, vertex1.y, vertex1.z);
        glm::vec3 position2(vertex2.x, vertex2.y, vertex2.z);

        glm::vec3 normal = glm::normalize(glm::cross(position1 - position0, position2 - position0));

        tv_flat_normals[i] = normal;
        tv_flat_normals[i + 1] = normal;
        tv_flat_normals[i + 2] = normal;

        v_smooth_normals[v0] += normal;
        v_smooth_normals[v1] += normal;
        v_smooth_normals[v2] += normal;
    }

    // Normalize v_smooth_normals
    for (std::size_t i = 0; i < v_smooth_normals.size(); ++i)
        v_smooth_normals[i] = glm::normalize(v_smooth_normals[i]);

    // If pmesh_ has per-vertex normals, then just use them.
    if (pmesh_->HasNormals())
        memcpy(&v_smooth_normals[0], &pmesh_->mNormals[0], sizeof(pmesh_->mNormals[0]) * pmesh_->mNumVertices);

    // Set tv_smooth_normals from v_smooth_normals
    tv_smooth_normals.resize(tv_indices_.size());
    for (unsigned int i = 0; i < tv_indices_.size(); ++i)
    {
        unsigned int vertexIndex = tv_indices_[i];
        glm::vec3 smoothNormal = v_smooth_normals[vertexIndex];
        tv_smooth_normals[i] = smoothNormal;
    }

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    if (shading_type == kSmooth)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_smooth_normals.size(), &tv_smooth_normals[0], GL_STATIC_DRAW);
    }
    else if (shading_type == kFlat)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * tv_flat_normals.size(), &tv_flat_normals[0], GL_STATIC_DRAW);
    }
}

void Mesh::set_gl_buffers(ShadingType shading_type)
{
    set_gl_position_buffer_();
    if (pmesh_->HasVertexColors(0))
        set_gl_color_buffer_(0);
    set_gl_normal_buffer_(shading_type);
}

void Mesh::draw(int loc_a_position, int loc_a_normal)
{
    // TODO : draw a triangular mesh
    //          glBindBuffer() with position_buffer_
    //          glEnableVertexAttribArray() for loc_a_position
    //          glVertexAttribPointer() by reusing GPU data in loc_a_position
    //
    //          glBindBuffer() with normal_buffer_
    //          glEnableVertexAttribArray() for loc_a_normal
    //          glVertexAttribPointer() by reusing GPU data in loc_a_normal
    //
    //          glDrawArrays(GL_TRIANGLES, ...)
    //
    //          glDisableVertexAttribArray() for loc_a_position & loc_a_normal
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
    glEnableVertexAttribArray(loc_a_position);
    glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glEnableVertexAttribArray(loc_a_normal);
    glVertexAttribPointer(loc_a_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, tv_indices_.size());

    glDisableVertexAttribArray(loc_a_position);
    glDisableVertexAttribArray(loc_a_normal);
}

void Mesh::print_info()
{
    std::cout << "print mesh info" << std::endl;

    std::cout << "num vertices " << pmesh_->mNumVertices << std::endl;
    for (unsigned int i = 0; i < pmesh_->mNumVertices; ++i)
    {
        aiVector3D vertex = pmesh_->mVertices[i];
        std::cout << "  vertex  (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;

        if (pmesh_->mColors[0] != NULL)
        {
            aiColor4D color = pmesh_->mColors[0][i];
            std::cout << "  color  (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")" << std::endl;
        }
    }
}