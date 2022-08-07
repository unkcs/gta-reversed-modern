#include "StdInc.h"

#include "EventDanger.h"

void CEventDanger::InjectHooks() {
    RH_ScopedClass(CEventDanger);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B2600);
    RH_ScopedVirtualInstall(AffectsPed, 0x4B5470);
    RH_ScopedVirtualInstall(AffectsPedGroup, 0x4B54E0);
    RH_ScopedVirtualInstall(GetSourceEntity, 0x4B2700);
}

CEventDanger* CEventDanger::Constructor(CEntity* dangerFrom, float dangerRadius) { this->CEventDanger::CEventDanger(dangerFrom, dangerRadius); return this; }
bool CEventDanger::AffectsPed(CPed* ped) { return CEventDanger::AffectsPed_Reversed(ped); }
bool CEventDanger::AffectsPedGroup(CPedGroup* pedGroup) { return CEventDanger::AffectsPedGroup_Reversed(pedGroup); }
CEntity* CEventDanger::GetSourceEntity() const { return CEventDanger::GetSourceEntity_Reversed(); }

// 0x4B2600
CEventDanger::CEventDanger(CEntity* dangerFrom, float dangerRadius) : CEventEditableResponse() {
    m_dangerFrom = dangerFrom;
    CEntity::SafeRegisterRef(m_dangerFrom);
    m_dangerRadius = dangerRadius;
}

CEventDanger::~CEventDanger() {
    CEntity::SafeCleanUpRef(m_dangerFrom);
}

// 0x4B5470
bool CEventDanger::AffectsPed_Reversed(CPed* ped) {
    if (m_dangerFrom && m_dangerFrom != ped->m_pVehicle) {
        CVector2D distance = ped->GetPosition() - m_dangerFrom->GetPosition();
        if (m_dangerRadius * m_dangerRadius >= distance.SquaredMagnitude())
            return ped->IsAlive();
    }
    return false;
}

// 0x4B54E0
bool CEventDanger::AffectsPedGroup_Reversed(CPedGroup* pedGroup) {
    if (GetSourceEntity() && GetSourceEntity()->IsPed()) {
        CPed* leader = pedGroup->GetMembership().GetLeader();
        if (leader) {
            CVector2D distance = leader->GetPosition() - m_dangerFrom->GetPosition();
            return distance.SquaredMagnitude() <= m_dangerRadius * m_dangerRadius;
        }
    }
    return false;
}

// 0x4B2700
CEntity* CEventDanger::GetSourceEntity_Reversed() const {
    if (m_dangerFrom && !m_dangerFrom->IsPed() && m_dangerFrom->IsVehicle()) {
        CVehicle* vehicle = m_dangerFrom->AsVehicle();
        if (vehicle->m_pDriver)
            return vehicle->m_pDriver;
    }
    return m_dangerFrom;
}
