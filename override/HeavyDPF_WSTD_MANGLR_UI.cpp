/**
 * Copyright (c) Wasted Audio 2023 - GPL-3.0-or-later
 */

#include "DistrhoUI.hpp"
#ifdef DISTRHO_OS_WASM
#include "DistrhoStandaloneUtils.hpp"
#endif
#include "ResizeHandle.hpp"
#include "veramobd.hpp"
#include "wstdcolors.hpp"

START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------
enum HeavyParams {
    CRSHR,
    FLDR,
    GAIN,
    LMTR,
    MIX,
    SMTHR,
    SQNC,
};

class ImGuiPluginUI : public UI
{
    int fcrshr = 512;
    float ffldr = 1.0f;
    float fgain = 0.0f;
    bool flmtr = 1.0f != 0.0f;
    float fmix = 50.0f;
    float fsmthr = 1.0f;
    int fsqnc = 0.0;

    ResizeHandle fResizeHandle;

    // ----------------------------------------------------------------------------------------------------------------

public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */
    ImGuiPluginUI()
        : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true),
          fResizeHandle(this)
    {
        setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true);

        ImGuiIO& io(ImGui::GetIO());

        ImFontConfig fc;
        fc.FontDataOwnedByAtlas = true;
        fc.OversampleH = 1;
        fc.OversampleV = 1;
        fc.PixelSnapH = true;

        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 16.0f * getScaleFactor(), &fc);
        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 21.0f * getScaleFactor(), &fc);
        io.Fonts->AddFontFromMemoryCompressedTTF((void*)veramobd_compressed_data, veramobd_compressed_size, 12.5f * getScaleFactor(), &fc);
        io.Fonts->Build();
        io.FontDefault = io.Fonts->Fonts[1];

        fResizeHandle.hide();
    }

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

   /**
      A parameter has changed on the plugin side.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override
    {
        switch (index) {
            case CRSHR:
                fcrshr = value;
                break;
            case FLDR:
                ffldr = value;
                break;
            case GAIN:
                fgain = value;
                break;
            case LMTR:
                flmtr = value != 0.0f;
                break;
            case MIX:
                fmix = value;
                break;
            case SMTHR:
                fsmthr = value;
                break;
            case SQNC:
                fsqnc = value;
                break;

            default: return;
        }

        repaint();
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Widget Callbacks

    void showCrshr(int crshstep, float hundred, float KnobFlags)
    {
        auto intense = (pow((float)fcrshr * -1.0f + 514.0f, 2.0f) / 2500.0f - 20.0f) / 5.0f;
        auto CrshrActive = ColorBright(Green, intense);
        auto CrshrHovered = ColorBright(GreenBr, intense);

        ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)CrshrActive);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)CrshrHovered);
        if (ImGuiKnobs::KnobInt(
            "Crshr", &fcrshr, 2, 512, crshstep, "%i",
            ImGuiKnobVariant_SteppedTick, hundred, KnobFlags, 9))
        {
            if (ImGui::IsItemActivated())
            {
                editParameter(CRSHR, true);
                if (ImGui::IsMouseDoubleClicked(0))
                    fcrshr = 512;
            }
            setParameterValue(CRSHR, fcrshr);
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine();
    }

    void showFldr(float elevstep, float hundred, float KnobFlags)
    {
        auto intense = (ffldr * 7.5f - 20.0f) / 5.0f;
        auto FldrActive  = ColorBright(Red, intense);
        auto FldrHovered = ColorBright(RedBr, intense);

        ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)FldrActive);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)FldrHovered);
        if (ImGuiKnobs::Knob(
            "Fldr", &ffldr, 1.0f, 13.37f, elevstep, "%.2f",
            ImGuiKnobVariant_SteppedTick, hundred, KnobFlags, 11))
        {
            if (ImGui::IsItemActivated())
            {
                editParameter(FLDR, true);
                if (ImGui::IsMouseDoubleClicked(0))
                    ffldr = 1.0f;
            }
            setParameterValue(FLDR, ffldr);
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine();
    }

    void showSmthr(float elevstep, float hundred, float KnobFlags)
    {
        auto intense = (fsmthr * 7.5f - 20.0f) / 5.0f;
        auto SmthrActive  = ColorBright(Blue, intense);
        auto SmthrHovered = ColorBright(BlueBr, intense);

        ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)SmthrActive);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)SmthrHovered);
        if (ImGuiKnobs::Knob(
            "Smthr", &fsmthr, 1.0f, 13.37f, elevstep, "%.2f",
            ImGuiKnobVariant_SteppedTick, hundred, KnobFlags, 11))
        {
            if (ImGui::IsItemActivated())
            {
                editParameter(SMTHR, true);
                if (ImGui::IsMouseDoubleClicked(0))
                    fsmthr = 1.0f;
            }
            setParameterValue(SMTHR, fsmthr);
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine();
    }

   /**
      ImGui specific onDisplay function.
    */
    void onImGuiDisplay() override
    {
        const float width = getWidth();
        const float height = getHeight();
        const float margin = 0.0f;
        auto scaleFactor = getScaleFactor();
        #ifdef DISTRHO_OS_WASM
        static bool inputActive = false;
        #endif

        ImGui::SetNextWindowPos(ImVec2(margin, margin));
        ImGui::SetNextWindowSize(ImVec2(width - 2 * margin, height - 2 * margin));

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.ChildBorderSize = 1.0f * scaleFactor;

        style.Colors[ImGuiCol_TitleBgActive] = (ImVec4)WstdTitleBgActive;
        style.Colors[ImGuiCol_WindowBg] = (ImVec4)WstdWindowBg;

        ImGuiIO& io(ImGui::GetIO());
        ImFont* defaultFont = ImGui::GetFont();
        ImFont* titleBarFont = io.Fonts->Fonts[2];
        ImFont* mediumFont = io.Fonts->Fonts[3];

        auto intense = (20.0f + (4.0f * fgain)) / 5.0f;

        auto SyncSw          = ColorBright(WhiteDr, intense);
        auto SyncGr          = ColorBright(Grey, intense);
        auto SyncGrHovered   = ColorBright(GreyBr, intense);
        auto SyncAct         = ColorBright(GreenDr, intense);
        auto SyncActHovered  = ColorBright(Green, intense);
        auto GainActive      = ColorBright(Green, intense);
        auto GainHovered     = ColorBright(GreenBr, intense);
        auto MixActive       = ColorMix(Green, Yellow, intense, fmix);
        auto MixHovered      = ColorMix(GreenBr, YellowBr, intense, fmix);

        const float hundred = 100 * scaleFactor;
        const float toggleWidth = 20 * scaleFactor;
        const float comboWidth = 42 * scaleFactor;

        auto crshstep = 8;
        auto elevstep = 0.1f;
        auto percstep = 1.0f;
        auto dbstep   = 0.1f;

        if (io.KeyShift)
        {
            crshstep = 1;
            elevstep = 0.01f;
            percstep = 0.1f;
            dbstep = 0.01f;
        }

        const char* sqnc_list[6] = {
            "C~F~S",
            "C~S~F",
            "F~C~S",
            "F~S~C",
            "S~C~F",
            "S~F~C",
        };

        ImGui::PushFont(titleBarFont);
        if (ImGui::Begin("WSTD MANGLR", nullptr, ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoCollapse))
        {
            ImGui::Dummy(ImVec2(0.0f, 6.0f * scaleFactor));
            ImGui::PushFont(defaultFont);

            #ifdef DISTRHO_OS_WASM
            if (!inputActive)
            {
                ImGui::OpenPopup("Activate");
            }

            if (ImGui::BeginPopupModal("Activate", nullptr, ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoMove))
            {
                if (ImGui::Button("OK", ImVec2(80, 0)))
                {
                    requestAudioInput();
                    inputActive = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            #endif

            auto ImGuiKnob_Flags = ImGuiKnobFlags_DoubleClickReset + ImGuiKnobFlags_ValueTooltip + ImGuiKnobFlags_NoInput + ImGuiKnobFlags_ValueTooltipHideOnClick;
            auto ImGuiKnob_FlagsLog = ImGuiKnob_Flags + ImGuiKnobFlags_Logarithmic;

            ImGui::Dummy(ImVec2(5.0f * scaleFactor, 0.0f));
            ImGui::SameLine();

            ImGui::BeginGroup();
            {
                ImGui::Dummy(ImVec2(0.0f, 9.0f * scaleFactor));

                ImGui::PushStyleColor(ImGuiCol_Text,            TextClr);
                ImGui::PushStyleColor(ImGuiCol_FrameBg,         (ImVec4)ColorMix(WstdWindowBg, BlueDr, 0.5f, 50.0f));
                ImGui::PushStyleColor(ImGuiCol_Header,          (ImVec4)BlueDr);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered,   (ImVec4)Blue);
                ImGui::PushStyleColor(ImGuiCol_HeaderActive,    (ImVec4)BlueBr);
                ImGui::PushFont(mediumFont);
                if (ImGui::BeginListBox("##Sqnc", ImVec2(comboWidth, 101 * scaleFactor)))
                {
                    for (int n = 0; n < 6; n++)
                    {
                        bool is_selected = (fsqnc == n);
                        if (ImGui::Selectable(sqnc_list[n], is_selected))
                        {
                            fsqnc = n;
                            editParameter(SQNC, true);
                            setParameterValue(SQNC, fsqnc);
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndListBox();
                }
                ImGui::PopStyleColor(5);
                ImGui::PopFont();
            }
            ImGui::EndGroup();
            ImGui::SameLine();

            ImGui::Dummy(ImVec2(5.0f * scaleFactor, 0.0f));
            ImGui::SameLine();

            {
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoScrollWithMouse;
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                ImGui::BeginChild("FX", ImVec2(333 * scaleFactor, 127 * scaleFactor), true, window_flags);

                switch (fsqnc) {
                    case 0:
                        showCrshr(crshstep, hundred, ImGuiKnob_FlagsLog);
                        showFldr(elevstep, hundred, ImGuiKnob_Flags);
                        showSmthr(elevstep, hundred, ImGuiKnob_Flags);
                        break;
                    case 1:
                        showCrshr(crshstep, hundred, ImGuiKnob_FlagsLog);
                        showSmthr(elevstep, hundred, ImGuiKnob_Flags);
                        showFldr(elevstep, hundred, ImGuiKnob_Flags);
                        break;
                    case 2:
                        showFldr(elevstep, hundred, ImGuiKnob_Flags);
                        showCrshr(crshstep, hundred, ImGuiKnob_FlagsLog);
                        showSmthr(elevstep, hundred, ImGuiKnob_Flags);
                        break;
                    case 3:
                        showFldr(elevstep, hundred, ImGuiKnob_Flags);
                        showSmthr(elevstep, hundred, ImGuiKnob_Flags);
                        showCrshr(crshstep, hundred, ImGuiKnob_FlagsLog);
                        break;
                    case 4:
                        showSmthr(elevstep, hundred, ImGuiKnob_Flags);
                        showCrshr(crshstep, hundred, ImGuiKnob_FlagsLog);
                        showFldr(elevstep, hundred, ImGuiKnob_Flags);
                        break;
                    case 5:
                        showSmthr(elevstep, hundred, ImGuiKnob_Flags);
                        showFldr(elevstep, hundred, ImGuiKnob_Flags);
                        showCrshr(crshstep, hundred, ImGuiKnob_FlagsLog);
                        break;
                }

                ImGui::EndChild();
                ImGui::PopStyleVar();
            }
            ImGui::SameLine();

            ImGui::Dummy(ImVec2(5.0f * scaleFactor, 0.0f));
            ImGui::SameLine();


            ImGui::BeginGroup();
            {
                ImGui::Dummy(ImVec2(0.0f, 4.0f * scaleFactor));

                ImGui::BeginGroup();
                {
                    // Title text
                    ImGui::PushStyleColor(ImGuiCol_Text, TextClr);
                    CenterTextX("Lmtr", toggleWidth);
                    ImGui::PopStyleColor();

                    ImGui::Dummy(ImVec2(0.0f, 35.0f) * scaleFactor);

                    // knob
                    ImGui::PushStyleColor(ImGuiCol_Text,            (ImVec4)SyncSw);

                    // inactive colors
                    ImGui::PushStyleColor(ImGuiCol_FrameBg,         (ImVec4)SyncGr);
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,  (ImVec4)SyncGrHovered);

                    // active colors
                    ImGui::PushStyleColor(ImGuiCol_Button,          (ImVec4)SyncAct);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)SyncActHovered);
                    if (ImGui::Toggle("##Lmtr", &flmtr, ImGuiToggleFlags_Animated))
                    {
                        if (ImGui::IsItemActivated())
                        {
                            editParameter(LMTR, true);
                            setParameterValue(LMTR, flmtr);
                        }
                    }
                    ImGui::PopStyleColor(5);
                }
                ImGui::EndGroup();
                ImGui::SameLine();

                ImGui::BeginGroup();
                {
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)GainActive);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)GainHovered);
                    if (ImGuiKnobs::Knob(
                        "Gain", &fgain, -20.0f, 0.0f, dbstep, "%.2fdB", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_Flags, 5))
                    {
                        if (ImGui::IsItemActivated())
                        {
                            editParameter(GAIN, true);
                            if (ImGui::IsMouseDoubleClicked(0))
                                fgain = 0.0f;
                        }
                        setParameterValue(GAIN, fgain);
                    }
                    ImGui::PopStyleColor(2);
                }
                ImGui::EndGroup();
                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_ButtonActive,    (ImVec4)MixActive);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   (ImVec4)MixHovered);
                if (ImGuiKnobs::Knob(
                    "Mix", &fmix, 0.0f, 100.0f, percstep, "%.1f%%", ImGuiKnobVariant_SteppedTick, hundred, ImGuiKnob_Flags, 11))
                {
                    if (ImGui::IsItemActivated())
                    {
                        editParameter(MIX, true);
                        if (ImGui::IsMouseDoubleClicked(0))
                            fmix = 50.0f;
                    }
                    setParameterValue(MIX, fmix);
                }
                ImGui::PopStyleColor(2);
            }
            ImGui::EndGroup();

            if (ImGui::IsItemDeactivated())
            {
                editParameter(CRSHR, false);
                editParameter(FLDR, false);
                editParameter(GAIN, false);
                editParameter(LMTR, false);
                editParameter(MIX, false);
                editParameter(SMTHR, false);
                editParameter(SQNC, false);
            }

            ImGui::PopFont();
        }
        ImGui::PopFont();
        ImGui::End();
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImGuiPluginUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new ImGuiPluginUI();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
