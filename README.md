# Tower Defense ++｜C++ 塔防遊戲

使用 **C++ 與 SDL2** 製作的期末專題，開發環境為 **Visual Studio 2022**。玩家透過配置不同類型的防禦塔、升級火力與管理金錢，阻擋持續出現的敵人，挑戰更高的波次。

## 遊戲介紹

遊戲以 960 × 640 的視窗呈現。玩家初始擁有 **120 金錢**與 **20 生命值**，擊敗敵人可獲得金錢；敵人抵達終點時，則會依類型扣除生命值。當生命值降至 0 或以下，遊戲結束，可按任意鍵重新開始。

敵人數量與生命值會隨波次增加。清除當前波次的所有敵人後，遊戲會自動進入下一波。

## 主要功能

- **五種防禦塔**：基本塔、狙擊塔、範圍塔、緩速塔與雷射塔。
- **防禦塔升級與出售**：升級可提升傷害、射程及攻擊速度；出售可回收原始建造費用的 50%。
- **多種敵人**：普通、快速、護盾、飛行、隱形敵人，以及三種 Boss。
- **自動攻擊與彈體效果**：防禦塔搜尋射程內的目標，包含範圍傷害、減速及雷射視覺效果。
- **遊戲狀態管理**：主選單、遊戲中、暫停與遊戲結束。
- **速度切換**：支援 1 倍、2 倍與 4 倍速。
- **基本存讀檔**：將部分遊戲狀態寫入 `save.dat`，並可在遊戲中讀取。

## 操作方式

| 操作 | 功能 |
| --- | --- |
| 主選單按任意鍵 | 開始遊戲 |
| 滑鼠左鍵點選下方塔種按鈕，再點選地圖位置 | 建造防禦塔，需有足夠金錢 |
| 滑鼠左鍵點選已建造的塔 | 選取該塔，查看資訊與射程 |
| 選取塔後點選 `Upgrade` | 花費金錢升級 |
| 選取塔後點選 `Sell` | 出售並回收部分金錢 |
| 選取塔後點選空白處 | 取消選取；再次點選位置即可建造 |
| `P` | 暫停／繼續 |
| `F` | 循環切換 1 → 2 → 4 → 1 倍速 |
| `S` | 儲存至目前工作目錄的 `save.dat` |
| `L` | 讀取目前工作目錄的 `save.dat` |
| `Next` 按鈕 | 場上沒有敵人時，手動切換至下一波 |
| 遊戲結束後按任意鍵 | 重新開始 |
| 關閉遊戲視窗 | 結束程式 |

暫停時只接受 `P` 繼續遊戲與關閉視窗；倍速、存檔及讀檔需在遊戲進行中操作。

## 防禦塔

| 類型 | 建造費用 | 特性 |
| --- | ---: | --- |
| Basic／基本塔 | 40 | 單體攻擊，適合作為初期防線 |
| Sniper／狙擊塔 | 70 | 傷害高、射程長，但攻擊間隔較長 |
| Splash／範圍塔 | 65 | 彈體命中後，對附近敵人造成範圍傷害 |
| Slow／緩速塔 | 55 | 造成傷害並暫時降低敵人移動速度；飛行敵人不受減速影響 |
| Laser／雷射塔 | 80 | 攻擊間隔短，具有蓄能、光束及淡出效果；無法鎖定隱形敵人 |

## 開發環境與依賴

| 項目 | 專案設定 |
| --- | --- |
| 作業系統 | Windows |
| 開發工具 | Visual Studio 2022 |
| 編譯工具集 | MSVC v143 |
| Windows SDK | Windows 10 SDK（依專案設定） |
| 建置平台 | x64 |
| SDL2 | [2.30.8](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.8)，視窗、事件處理與繪圖 |
| SDL2_image | [2.8.2](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.2)，PNG 圖片載入 |
| SDL2_ttf | [2.22.0](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.22.0)，文字繪製 |

上列函式庫版本依目前 `.vcxproj` 設定列出。函式庫需另外安裝，未包含在原始碼與 `assets` 中。

## 建置與執行

### 1. 準備開發環境

安裝 Visual Studio 2022，並在 Visual Studio Installer 中選取 **「使用 C++ 的桌面開發」**工作負載，確認包含 MSVC v143 與 Windows SDK。

### 2. 準備 SDL 函式庫

從上方官方版本頁面下載 Windows **Visual C++ 開發套件**（檔名含 `devel` 與 `VC`），解壓縮後依目前專案設定放置為：

```text
C:\libs\
├── SDL2-2.30.8\
├── SDL2_image-2.8.2\
└── SDL2_ttf-2.22.0\
```

每個套件目錄下應能找到 `include` 與 `lib\x64`。若使用其他安裝位置，請在專案屬性中，對準備使用的組態與 x64 平台調整：

- **C/C++ → 一般 → 其他 Include 目錄**：各套件的 `include`。
- **連結器 → 一般 → 其他程式庫目錄**：各套件的 `lib\x64`。
- **連結器 → 輸入 → 其他相依性**：保留 `SDL2main.lib`、`SDL2.lib`、`SDL2_image.lib`、`SDL2_ttf.lib` 與 `shell32.lib`。

### 3. 開啟方案並建置

1. 下載或 clone 此儲存庫。
2. 用 Visual Studio 2022 開啟**儲存庫根目錄**的 `final project.sln`。
3. 選擇 `Debug | x64` 或 `Release | x64`。
4. 執行「建置方案」。

目前 Win32 組態也指向 x64 程式庫，因此請使用 **x64**，以避免架構不符。

### 4. 配置執行所需 DLL

將各開發套件 `lib\x64` 內的執行用 DLL 複製到產生的 `.exe` 所在資料夾，至少包括：

```text
SDL2.dll
SDL2_image.dll
SDL2_ttf.dll
```

若套件另外附有相依 DLL，請一併放置。DLL 位元架構需與 x64 執行檔一致。

### 5. 設定圖片與字型路徑

程式使用相對路徑 `assets/...` 載入圖片。在 Visual Studio 的專案屬性中，將 **「偵錯 → 工作目錄」**設定為：

```text
$(ProjectDir)
```

如此便會從內層 `final project` 資料夾讀取 `assets`，存檔也會寫入該資料夾。若直接雙擊 `.exe` 執行，請將完整的 `assets` 資料夾複製到 `.exe` 旁。

文字目前使用 `C:\Windows\Fonts\msjh.ttc`（微軟正黑體）。若系統沒有此字型，請修改 `Game.cpp` 中的 `TTF_OpenFont` 路徑，指向可用的字型檔案。

完成以上設定後，即可在 Visual Studio 中按 `F5` 或 `Ctrl + F5` 啟動。

## 專案結構

以下列出主要原始碼與文件；編譯輸出及暫存資料夾省略。

```text
.
├── README.md
├── final project.sln                  # 外層方案入口
├── final project/
│   ├── final project.vcxproj          # C++ 專案與建置設定
│   ├── final project.vcxproj.filters  # Visual Studio 檔案分類
│   ├── main.cpp                      # SDL 初始化與遊戲主迴圈
│   ├── Constants.hpp                 # 視窗尺寸、向量及距離運算
│   ├── Game.cpp / Game.hpp            # 遊戲狀態、輸入、波次、UI 與存讀檔
│   ├── Enemy.cpp / Enemy.hpp          # 敵人屬性、移動與傷害處理
│   ├── Tower.cpp / Tower.hpp          # 防禦塔類型、費用與升級
│   ├── Projectile.cpp / Projectile.hpp # 彈體、命中效果與雷射動畫
│   ├── Sound.hpp                     # 音效介面，目前為空實作
│   └── assets/                       # 背景、防禦塔與敵人圖片
├── 期末專題報告.pdf
├── Tower_Defense_Class_Diagram_Visual.pdf
└── Tower_Defense_Logic_Flowcharts.pdf
```

## 專題文件

- [期末專題報告](期末專題報告.pdf)
- [類別圖](Tower_Defense_Class_Diagram_Visual.pdf)
- [邏輯流程圖](Tower_Defense_Logic_Flowcharts.pdf)

## 目前限制與後續方向

- **環境設定**：函式庫與字型使用固定 Windows 路徑，移至其他電腦時需確認設定。
- **存讀檔**：目前保存部分狀態；讀檔時會重新設定波次生成資訊，未完整還原防禦塔升級後的戰鬥屬性、敵人路徑進度與彈體狀態。
- **音效**：`Sound.hpp` 已保留呼叫介面，尚未實作音效播放。
- **後續可延伸方向**：改善存讀檔完整性、整合音效、增加地圖，以及將依賴與資源路徑改為較容易移植的配置。
