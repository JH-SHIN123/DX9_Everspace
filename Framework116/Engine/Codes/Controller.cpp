#include "Controller.h"

USING(Engine)

CController::CController(LPDIRECT3DDEVICE9 pDevice)
    : CComponent(pDevice)
{
}

CController::CController(const CController& other)
    : CComponent(other)
{
}

void CController::Update_Controller()
{
    m_dwKey = 0;
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        m_dwKey |= KEY_LEFT;
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        m_dwKey |= KEY_RIGHT;
    if (GetAsyncKeyState(VK_UP) & 0x8000)
        m_dwKey |= KEY_UP;
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        m_dwKey |= KEY_DOWN;
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        m_dwKey |= KEY_LBUTTON;
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        m_dwKey |= KEY_SPACE;
    if (GetAsyncKeyState(VK_RETURN) & 0x8000)
        m_dwKey |= KEY_ENTER;
    if (GetAsyncKeyState('1') & 0x8000)
        m_dwKey |= KEY_1;
    if (GetAsyncKeyState('2') & 0x8000)
        m_dwKey |= KEY_2;
    if (GetAsyncKeyState('3') & 0x8000)
        m_dwKey |= KEY_3;
    if (GetAsyncKeyState(VK_F1) & 0x8000)
        m_dwKey |= KEY_F1;
    if (GetAsyncKeyState(VK_F2) & 0x8000)
        m_dwKey |= KEY_F2;
    if (GetAsyncKeyState(VK_F3) & 0x8000)
        m_dwKey |= KEY_F3;
    if (GetAsyncKeyState('R') & 0x8000)
        m_dwKey |= KEY_R;
    if (GetAsyncKeyState('P') & 0x8000)
        m_dwKey |= KEY_P;
	if (GetAsyncKeyState('F') & 0x8000)
		m_dwKey |= KEY_F;
}

bool CController::Key_Up(DWORD dwKey)
{
    //0000 0011 & 
        //0000 0001
        //0000 0001
    if (m_dwKey & dwKey)
    {
        m_dwKeyUP |= dwKey;
        return false;
    }
    // 0000 0011
    // 0000 0001&
    // 0000 0001
    else if (m_dwKeyUP & dwKey)
    {
        //0000 0011 KeyUp
        //0000 0001 ^ dwKey
        //0000 0010

        m_dwKeyUP ^= dwKey;
        return true;
    }
    return false;
}

bool CController::Key_Down(DWORD dwKey)
{
    if ((m_dwKey & dwKey) && !(m_dwKeyDOWN & dwKey))
    {
        m_dwKeyDOWN |= dwKey;
        return true;
    }
    else if (!(m_dwKey & dwKey) && (m_dwKeyDOWN & dwKey))
    {
        m_dwKeyDOWN ^= dwKey;
        return false;
    }
    return false;
}

bool CController::Key_Pressing(DWORD dwKey)
{
    if (m_dwKey & dwKey)
        return true;
    return false;
}

HRESULT CController::Ready_Component_Prototype()
{
    CComponent::Ready_Component_Prototype();

    return S_OK;
}

HRESULT CController::Ready_Component(void* pArg)
{
    CComponent::Ready_Component(pArg);

    return S_OK;
}

CController* CController::Create(LPDIRECT3DDEVICE9 pDevice)
{
    CController* pInstance = new CController(pDevice);
    if (FAILED(pInstance->Ready_Component_Prototype()))
    {
        PRINT_LOG(L"Error", L"Failed To Create Texture");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CController::Clone(void* pArg)
{
    CController* pClone = new CController(*this);
    if (FAILED(pClone->Ready_Component(pArg)))
    {
        PRINT_LOG(L"Error", L"Failed To Clone Texture");
        Safe_Release(pClone);
    }

    return pClone;
}

void CController::Free()
{
    CComponent::Free();
}
