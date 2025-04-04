#include "tools/tools.hpp"
#include <glm/gtx/euler_angles.hpp>

using namespace std;

// Courtesy of: http://stackoverflow.com/questions/5878775/how-to-find-and-replace-string
string bee::StringReplace(const string& subject, const string& search, const string& replace)
{
    string result(subject);
    size_t pos = 0;

    while ((pos = subject.find(search, pos)) != string::npos)
    {
        result.replace(pos, search.length(), replace);
        pos += search.length();
    }

    return result;
}

string bee::StringRemove(const string& subject, const string& toRemove)
{
    string result(subject);
    size_t foundAtPos = 0;
    const size_t toRemoveLength = toRemove.length();

    while ((foundAtPos = subject.find(toRemove, foundAtPos)) != string::npos)
    {
        result.erase(foundAtPos, toRemoveLength);
        foundAtPos += toRemoveLength;
    }

    return result;
}

bool bee::StringEndsWith(const string& subject, const string& suffix)
{
    // Early out test:
    if (suffix.length() > subject.length())
        return false;

    // Resort to difficult to read C++ logic:
    return subject.compare(subject.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool bee::StringStartsWith(const string& subject, const std::string& prefix)
{
    // Early out, prefix is longer than the subject:
    if (prefix.length() > subject.length())
        return false;

    // Compare per character:
    for (size_t i = 0; i < prefix.length(); ++i)
        if (subject[i] != prefix[i])
            return false;

    return true;
}

constexpr int HALF_RAND = (RAND_MAX / 2);
float bee::RandomFloatBtwnMinus1and1()
{
    int const rn = rand();
    return static_cast<float>(rn - HALF_RAND) / static_cast<float>(HALF_RAND);
}

glm::quat bee::GetQuatFromDegrees(glm::vec3 degrees)
{
    const glm::vec3 radians = glm::radians(degrees);
    return radians; // quaternion accepts radians for its constructor
}

glm::vec3 bee::GetRotationInRadiansFromMatrix(const glm::mat4& matrix)
{
    auto rotationMatrix = glm::mat3(matrix);

    // Normalize the rotation matrix to remove the scaling factor
    for (int i = 0; i < 3; ++i)
    {
        rotationMatrix[i] /= glm::length(rotationMatrix[i]);
    }

    float x, y, z;

    y = asin(-rotationMatrix[2][0]);
    if (!are_floats_equal(cos(y), 0.f))
    {
        x = atan2(rotationMatrix[2][1], rotationMatrix[2][2]);
        z = atan2(rotationMatrix[1][0], rotationMatrix[0][0]);
    }
    else
    {
        x = atan2(-rotationMatrix[0][1], rotationMatrix[1][1]);
        z = 0;
    }

    return {x, y, z};
}

glm::vec2 bee::GetXZDirectionFromQuaternion(const glm::quat& quat)
{
    const glm::vec3 direction3D = glm::vec3(0.0f, 0.f, 1.f) * quat;
    return normalize(glm::vec2(-direction3D.x, direction3D.z));
}

glm::vec2 bee::WorldToScreenCoordinates(
    const glm::mat4& matrix, const glm::mat4& view, const glm::mat4& projection, const glm::vec2& windowSize)
{
    const glm::mat4 mvp = projection * view * matrix;
    const glm::vec4 worldCoordinates = {
        matrix[0][3],
        matrix[1][3],
        matrix[2][3],
        matrix[3][3],
    };
    const glm::vec4 clipCoordinates = mvp * worldCoordinates;
    const glm::vec3 ndcCoordinates = glm::vec3(clipCoordinates) / clipCoordinates.w;

    float x = (ndcCoordinates.x + 1.f) * windowSize.x / 2.f;
    float y = (1.f - ndcCoordinates.y) * windowSize.y / 2.f;

    return {x, y};
}
