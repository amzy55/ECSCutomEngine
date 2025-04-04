#pragma once
#include <string>

namespace bee
{
class Serializer
{
public:
    /**
     * \brief Serializes all entities and the specified components.
     */
    void Serialize() const;

    /**
     * \brief Deserializes the entities and their components.
     */
    void Deserialize() const;

    /**
     * \brief Checks to see it if finds the serialization file.
     * \return Whether or not it has found it.
     */
    [[nodiscard]] bool IsThereACerealFile() const;

private:
    const std::string m_serializationFileName = "serialized.cereal";
};
}
