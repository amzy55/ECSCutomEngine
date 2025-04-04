#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace bee
{

inline glm::vec3 to_vec3(const glm::vec2& vec) { return glm::vec3(vec.x, vec.y, 0.0f); }

inline glm::vec3 to_vec3(std::vector<double> array) { return glm::vec3((float)array[0], (float)array[1], (float)array[2]); }

inline glm::quat to_quat(std::vector<double> array)
{
    return glm::quat((float)array[3], (float)array[0], (float)array[1], (float)array[2]);
}

inline glm::vec4 to_vec4(std::vector<double> array)
{
    return glm::vec4((float)array[0], (float)array[1], (float)array[2], (float)array[3]);
}

/// Replace all occurrences of the search string with the replacement string.
///
/// @param subject The string being searched and replaced on, otherwise known as the haystack.
/// @param search The value being searched for, otherwise known as the needle.
/// @param replace The replacement value that replaces found search values.
/// @return a new string with all occurrences replaced.
///
std::string StringReplace(const std::string& subject, const std::string& search, const std::string& replace);

/**
 * \brief Deletes all occurrences of a string from another string if it finds it.
 * \param subject The string to search and get modified.
 * \param toRemove The string to remove from the subject.
 * \returns The modified string.
 */
std::string StringRemove(const std::string& subject, const std::string& toRemove);

/// Determine whether or not a string ends with the given suffix. Does
/// not create an internal copy.
///
/// @param subject The string being searched in.
/// @param prefix The string to search for.
/// @return a boolean indicating if the suffix was found.
///
bool StringEndsWith(const std::string& subject, const std::string& suffix);

/// Determine whether or not a string starts with the given prefix. Does
/// not create an internal copy.
///
/// @param subject The string being searched in.
/// @param prefix The string to search for.
/// @return a boolean indicating if the prefix was found.
///
bool StringStartsWith(const std::string& subject, const std::string& prefix);

/**
 * \brief Random number generation that uses rand() so it may not be thread safe.
 * \return a random float value between -1 and 1.
 */
float RandomFloatBtwnMinus1and1();

/**
 * \brief Returns a normalized quaternion from a vec3 of degrees
 */
glm::quat GetQuatFromDegrees(glm::vec3 degrees);

/**
 * \brief To safely compare floats.
 */
inline bool are_floats_equal(const float a, const float b, const float epsilon = 1e-6f) { return std::fabs(a - b) < epsilon; }

glm::vec3 GetRotationInRadiansFromMatrix(const glm::mat4& matrix);

glm::vec2 GetXZDirectionFromQuaternion(const glm::quat& quat);

glm::vec2 WorldToScreenCoordinates(
    const glm::mat4& matrix, const glm::mat4& view, const glm::mat4& projection, const glm::vec2& windowSize);

} // namespace bee