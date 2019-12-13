#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "../../util/util_time.h"

#include "dxvk_hud_renderer.h"

namespace dxvk::hud {

  /**
   * \brief HUD item
   *
   * A single named item in the HUD that
   * can be enabled by the user.
   */
  class HudItem : public RcObject {

  public:

    virtual ~HudItem();

    /**
     * \brief Updates the HUD item
     * \param [in] time Current time
     */
    virtual void update(
            dxvk::high_resolution_clock::time_point time);

    /**
     * \brief Renders the HUD
     *
     * \param [in] renderer HUD renderer
     * \param [in] position Base offset
     * \returns Base offset for next item
     */
    virtual HudPos render(
            HudRenderer&      renderer,
            HudPos            position) = 0;

  };


  /**
   * \brief HUD item set
   *
   * Manages HUD items.
   */
  class HudItemSet {

  public:

    HudItemSet();

    ~HudItemSet();

    /**
     * \brief Updates the HUD
     * Updates all enabled HUD items.
     */
    void update();

    /**
     * \brief Renders the HUD
     *
     * \param [in] renderer HUD renderer
     * \returns Base offset for next item
     */
    void render(
            HudRenderer&      renderer);

    /**
     * \brief Creates a HUD item if enabled
     *
     * \tparam T The HUD item type
     * \param [in] name HUD item name
     * \param [in] args Constructor arguments
     */
    template<typename T, typename... Args>
    void add(const char* name, Args... args) {
      bool enable = m_enableFull;

      if (!enable) {
        auto entry = m_enabled.find(name);
        enable = entry != m_enabled.end();
      }

      if (enable)
        m_items.push_back(new T(std::forward<Args>(args)...));
    }

  private:

    bool                            m_enableFull = false;
    std::unordered_set<std::string> m_enabled;
    std::vector<Rc<HudItem>>        m_items;

  };


  /**
   * \brief HUD item to display DXVK version
   */
  class HudVersionItem : public HudItem {

  public:

    HudPos render(
            HudRenderer&      renderer,
            HudPos            position);

  };


  /**
   * \brief HUD item to display the client API
   */
  class HudClientApiItem : public HudItem {

  public:

    HudClientApiItem(const Rc<DxvkDevice>& device);

    ~HudClientApiItem();

    HudPos render(
            HudRenderer&      renderer,
            HudPos            position);

  private:

    Rc<DxvkDevice> m_device;

  };


  /**
   * \brief HUD item to display device info
   */
  class HudDeviceInfoItem : public HudItem {

  public:

    HudDeviceInfoItem(const Rc<DxvkDevice>& device);

    ~HudDeviceInfoItem();

    HudPos render(
            HudRenderer&      renderer,
            HudPos            position);

  private:

    std::string m_deviceName;
    std::string m_driverVer;
    std::string m_vulkanVer;

  };


  /**
   * \brief HUD item to display the frame rate
   */
  class HudFpsItem : public HudItem {
    constexpr static int64_t UpdateInterval = 500'000;
  public:

    HudFpsItem();

    ~HudFpsItem();

    void update(dxvk::high_resolution_clock::time_point time);

    HudPos render(
            HudRenderer&      renderer,
            HudPos            position);

  private:

    uint32_t                                m_frameCount = 0;
    dxvk::high_resolution_clock::time_point m_lastUpdate
      = dxvk::high_resolution_clock::now();

    std::string m_frameRate;

  };


  /**
   * \brief HUD item to display the frame rate
   */
  class HudFrameTimeItem : public HudItem {
    constexpr static size_t NumDataPoints = 300;
  public:

    HudFrameTimeItem();

    ~HudFrameTimeItem();

    void update(dxvk::high_resolution_clock::time_point time);

    HudPos render(
            HudRenderer&      renderer,
            HudPos            position);

  private:

    dxvk::high_resolution_clock::time_point m_lastUpdate
      = dxvk::high_resolution_clock::now();

    std::array<float, NumDataPoints>  m_dataPoints  = {};
    uint32_t                          m_dataPointId = 0;

  };


  /**
   * \brief HUD item to display queue submissions
   */
  class HudSubmissionStatsItem : public HudItem {
    constexpr static int64_t UpdateInterval = 500'000;
  public:

    HudSubmissionStatsItem(const Rc<DxvkDevice>& device);

    ~HudSubmissionStatsItem();

    void update(dxvk::high_resolution_clock::time_point time);

    HudPos render(
            HudRenderer&      renderer,
            HudPos            position);

  private:

    Rc<DxvkDevice>  m_device;

    uint64_t        m_prevCounter = 0;
    uint64_t        m_diffCounter = 0;
    uint64_t        m_showCounter = 0;

    dxvk::high_resolution_clock::time_point m_lastUpdate
      = dxvk::high_resolution_clock::now();

  };


  /**
   * \brief HUD item to display draw call counts
   */
  class HudDrawCallStatsItem : public HudItem {
    constexpr static int64_t UpdateInterval = 500'000;
  public:

    HudDrawCallStatsItem(const Rc<DxvkDevice>& device);

    ~HudDrawCallStatsItem();

    void update(dxvk::high_resolution_clock::time_point time);

    HudPos render(
            HudRenderer&      renderer,
            HudPos            position);

  private:

    Rc<DxvkDevice>    m_device;

    DxvkStatCounters  m_prevCounters;
    uint64_t          m_frameCount = 0;

    uint64_t          m_gpCount = 0;
    uint64_t          m_cpCount = 0;
    uint64_t          m_rpCount = 0;

    dxvk::high_resolution_clock::time_point m_lastUpdate
      = dxvk::high_resolution_clock::now();

  };


  /**
   * \brief HUD item to display pipeline counts
   */
  class HudPipelineStatsItem : public HudItem {

  public:

    HudPipelineStatsItem(const Rc<DxvkDevice>& device);

    ~HudPipelineStatsItem();

    void update(dxvk::high_resolution_clock::time_point time);

    HudPos render(
            HudRenderer&      renderer,
            HudPos            position);

  private:

    Rc<DxvkDevice> m_device;

    uint64_t m_graphicsPipelines = 0;
    uint64_t m_computePipelines = 0;

  };


  /**
   * \brief HUD item to display memory usage
   */
  class HudMemoryStatsItem : public HudItem {

  public:

    HudMemoryStatsItem(const Rc<DxvkDevice>& device);

    ~HudMemoryStatsItem();

    void update(dxvk::high_resolution_clock::time_point time);

    HudPos render(
            HudRenderer&      renderer,
            HudPos            position);

  private:

    Rc<DxvkDevice>                    m_device;
    VkPhysicalDeviceMemoryProperties  m_memory;
    DxvkMemoryStats                   m_heaps[VK_MAX_MEMORY_HEAPS];

  };


  /**
   * \brief HUD item to display GPU load
   */
  class HudGpuLoadItem : public HudItem {
    constexpr static int64_t UpdateInterval = 500'000;
  public:

    HudGpuLoadItem(const Rc<DxvkDevice>& device);

    ~HudGpuLoadItem();

    void update(dxvk::high_resolution_clock::time_point time);

    HudPos render(
            HudRenderer&      renderer,
            HudPos            position);

  private:

    Rc<DxvkDevice> m_device;

    uint64_t m_prevGpuIdleTicks = 0;
    uint64_t m_diffGpuIdleTicks = 0;

    std::string m_gpuLoadString;

    dxvk::high_resolution_clock::time_point m_lastUpdate
      = dxvk::high_resolution_clock::now();

  };


  /**
   * \brief HUD item to display pipeline compiler activity
   */
  class HudCompilerActivityItem : public HudItem {
    constexpr static int64_t MinShowDuration = 1500;
  public:

    HudCompilerActivityItem(const Rc<DxvkDevice>& device);

    ~HudCompilerActivityItem();

    void update(dxvk::high_resolution_clock::time_point time);

    HudPos render(
            HudRenderer&      renderer,
            HudPos            position);

  private:

    Rc<DxvkDevice> m_device;

    bool m_show = false;

    dxvk::high_resolution_clock::time_point m_timeShown
      = dxvk::high_resolution_clock::now();

  };

}