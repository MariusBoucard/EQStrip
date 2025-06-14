#include <JuceHeader.h>

namespace GUI
{
    class VerticalMeterComponent : public Component
    {
    public:
        VerticalMeterComponent()
        {
        }

        void paint(Graphics& g) override;
        void setLevel(float inLevel);

    private:
        float mLevel = -48.0f;
        float mMinLevel = -48.0f;
        float mMaxLevel = 12.0f;
    };
}
