# Test: UE5 Action Combat Prototype

[English](#english) | [中文](#中文)

---

<a name="english"></a>
## English Version

An advanced Action-RPG prototype built with **Unreal Engine 5.7** and **C++**, focusing on high-performance character movement, state-driven combat, and modular gameplay systems.

### 🏗 Project Architecture
The project follows a decoupled, component-based architecture to ensure scalability and maintainability.

1. **Core Gameplay Framework**
   - **Character Layer**: Custom `AMyCharacter` implementing complex locomotion and combat states.
   - **Controller Layer**: Enhanced Input driven `ACharacterController` managing mapping contexts and action triggers.
   - **Attribute System**: A standalone `UAttributeComponent` managing health, stamina, and progression data.

2. **Interaction & Combat**
   - **Weapon System**: Frame-accurate collision detection using sweep-based box tracing. Supports `EquipRotationOffset` for weapon model orientation correction.
   - **Interface-Driven Interaction**: Uses `IHitInterface` to handle combat interactions across different actor types.
   - **Enemy AI**: Full `EEnemyState` FSM with perception, facing verification before attack, attack cooldown system, and 2D BlendSpace-driven locomotion. See [State Machine Flow](#state-machine-flow) below.
   - **Combat Distance System**: Three radii (`ChasingRadius`/`CombatingRadius`/`PatrolRadius`) control AI behavior transitions, with `AcceptanceRadius` compensating for target capsule radius.

### 🧠 Key Technical & Algorithmic Highlights

- **Precise Collision Sweeping**: Implements **Box Trace Sweep** between `OldCenter` and `CurrentCenter` to prevent "ghost swings" at high speeds.
- **Directional Locomotion**: Uses `DotProduct(Velocity, ActorForward)` to scale speed: Forward (1.0x), Strafe (0.8x), Backwards (0.65x).
- **Health Buffer Visuals**: Implements a delayed buffer bar effect for better visual clarity on damage received.
- **Enemy Attack Pipeline**: Combat state facing verification (DotProduct ±15°) before attack, with full movement lock during attack montage.
- **Upper Body Animation Layering**: Layered Blend Per Bone + Slot node for weapon arming/disarming while moving, controlled by transient `bIsArming` state.

---

<a name="中文"></a>
## 中文版本

这是一个基于 **Unreal Engine 5.7** 和 **C++** 开发的高级动作角色扮演游戏（ARPG）原型，核心侧重于高性能角色移动、状态驱动战斗系统以及模块化玩法架构。

### 🏗 项目架构
项目采用解耦的组件化架构，以确保系统的可扩展性和可维护性。

1. **核心玩法框架**
   - **角色层 (Character)**：自定义 `AMyCharacter` 类，实现了复杂的运动学逻辑与战斗状态机。
   - **控制器层 (Controller)**：基于增强输入（Enhanced Input）的 `ACharacterController`，管理映射上下文与动作触发。
   - **属性系统 (Attribute)**：独立的 `UAttributeComponent` 负责生命值、金币等数据管理，与表现层完全解耦。

2. **交互与战斗系统**
   - **武器系统**：通过记录前一帧位置并进行盒体扫掠（Box Trace Sweep），实现跨帧的精确碰撞检测，消除高速挥砍时的漏判。支持装备旋转偏移（`EquipRotationOffset`）修正不同武器模型的朝向差异。
   - **接口驱动交互**：利用 `IHitInterface` 统一处理不同类型 Actor（敌人、可破坏物）的受击效果、粒子与音效。
   - **敌人 AI**：基于 `EEnemyState` 状态机，支持完整的战斗流程：感知追击 → 面朝校验 → 攻击 → 冷却等待 → 硬直恢复。巡逻阶段使用平滑旋转张望，追击阶段使用 2D BlendSpace（Speed × Direction）驱动移动动画。攻击冷却从攻击开始计算，让追击时间重叠冷却，体感更紧凑。
   - **战斗距离系统**：三个半径（`ChasingRadius`/`CombatingRadius`/`PatrolRadius`）控制 AI 行为切换，`MoveToTarget` 的 `AcceptanceRadius` 补偿目标胶囊体半径以精确停在战斗范围内。

3. **环境与效果**
   - **破碎系统**：集成 Chaos 物理几何体集（Geometry Collections），实现环境的真实破坏效果。
   - **程序化生成 (PCG)**：利用 PCG 图表动态生成竞技场环境与地物布局。

### 🧠 核心技术与算法亮点

- **精确碰撞扫掠 (Weapon System)**
  为了防止在低帧率或高速挥剑时武器穿模而不产生判定，系统实现了**盒体扫掠检测**。通过计算刀刃在相邻两帧之间的中心位移路径，进行 `BoxTraceSingle` 判定，确保 100% 的命中可靠性。

- **方向敏感型运动算法 (Directional Locomotion)**
  移动速度并非固定值，而是根据移动方向与角色正前方的夹角动态缩放：
  - **算法**：`FVector::DotProduct(速度, 角色前方)`。
  - **正向移动 (>0.8)**：全速运行 (默认 300 / 冲刺 450)。
  - **横向平移 (-0.2 到 0.2)**：速度降至基准的 80%。
  - **后退移动 (<-0.8)**：速度降至基准的 65%，模拟真实的负重感。

- **血条缓冲视觉逻辑 (Health Buffer)**
  UI 实现了现代动作游戏中常见的”残影血条”效果：受击时主血条立即扣除，缓冲条经过短暂延迟后通过 `FMath::FInterpTo` 平滑追随，增强了受击时的视觉冲击力。

- **敌人攻击流水线 (Enemy Attack Pipeline)**
  战斗状态下先通过 `DotProduct` 校验面朝角度（±15°），满足条件才触发攻击。攻击蒙太奇期间完全锁定移动与旋转，结束后回到追击状态重新逼近。

- **上半身动画分层 (Upper Body Animation Layering)**
  通过 Layered Blend Per Bone + Slot 节点实现移动中拔刀/收刀动画，由瞬态变量 `bIsArming` 控制混合权重，与持久状态 `ArmWeaponState` 分离。

---

<a name="state-machine-flow"></a>
## 🔄 敌人状态机流转图 / Enemy State Machine Flow

### 状态总览 / States

| 状态 | 说明 |
|------|------|
| `EES_UnOccupied` | 初始状态，第一帧自动转为 Patrolling |
| `EES_Patrolling` | 在巡逻点之间移动，到达后张望等待 |
| `EES_Chasing` | 追击玩家，持续导航 |
| `EES_Combating` | 进入战斗范围，面朝校验后攻击 |
| `EES_Attacking` | 攻击蒙太奇播放中，锁定移动 |
| `EES_Stunned` | 受击硬直，锁定移动 |
| `EES_Dead` | 死亡演出，清理所有资源 |

### 流转图 / Flow Diagram

```
+--------------------------------------------+
|            Tick() every frame              |
|  +--------------------------------------+  |
|  | Guard: Dead/Stunned/Attacking -> skip|  |
|  | CheckCombatTarget() -> decide state  |  |
|  | switch(state) -> run state tick      |  |
|  +--------------------------------------+  |
+--------------------------------------------+

+-------------+  perception   +-------------+ enter CombatingR +-------------+
|  Patrolling |-------------->|   Chasing   |----------------->|  Combating  |
|             |<--------------|             |<-----------------|             |
+-------------+  beyond Chase +-------------+  beyond Combat R +------+------+
                 Radius                                   facing OK  |
                                                        +-------------+
                                                        v
                                                 +-------------+
                                                 |  Attacking  |
                                                 +------+------+
                                                        |
                                               OnAttackEnd()
                                               -> CheckCombatTarget()
                                                        |
                               +------------------------+
                               |
+------------------------------+------+
| target lost or out of range       |
| ChasingTarget = nullptr           |
+-----------------------------------+

Any state --(TakeDamage, alive)--> Stunned --(OnHitReactEnd)--> CheckCombatTarget()
Any state --(TakeDamage, dead) ---> Dead

+-------------+
|   Stunned   |--> OnHitReactEnd() -> CheckCombatTarget()
+-------------+

+-------------+
|  Attacking  |--> OnAttackEnd() -> CheckCombatTarget()
+-------------+
```

### 攻击冷却机制 / Attack Cooldown

```
Attack() called
  +-- bAttackOnCooldown = true
  +-- Timer: RandRange(MinAttackInterval, MaxAttackInterval)  // default 3~5s
  +-- SetEnemyState(EES_Attacking)
  +-- PlayAttackMontage("Attack1")

Montage end -> OnAttackEnd() -> CheckCombatTarget() -> may return to Combating
                                              |
                               but CanAttack() checks bAttackOnCooldown
                               during cooldown: face player and wait

Cooldown starts from Attack(), chase time overlaps cooldown -> tighter feel
```

### 关键方法职责 / Key Method Responsibilities

| 方法 | 调用时机 | 职责 |
|------|----------|------|
| `Tick()` | 每帧 | 守卫 + CheckCombatTarget + 状态 Tick |
| `CheckCombatTarget()` | Tick / 蒙太奇结束 | 根据距离决定 Patrolling/Chasing/Combating |
| `SetEnemyState()` | 状态切换时 | 退出旧状态清理 + 进入新状态初始化 |
| `Attack()` | Combating 状态下面朝目标 | 启动冷却 + 切 Attacking + 播蒙太奇 |
| `TakeDamage()` | 被攻击时 | 扣血 + 切 Stunned 或 Dead |
| `Die()` | 进入 Dead 状态 | 清 Timer + 停移动 + 关碰撞 + 播死亡动画 |

---

## 🚀 快速上手 / Getting Started

### 环境要求 (Requirements)
- **Unreal Engine 5.7+**
- **Visual Studio 2022** (需安装 C++ 游戏开发负载)

### 安装步骤 (Installation)
1. 将仓库克隆至你的虚幻项目文件夹。
2. 右键点击 `Test.uproject` -> **Generate Visual Studio project files**。
3. 打开 `Test.sln` 并编译 (Development Editor 模式)。
4. 通过 `Test.uproject` 启动编辑器。
