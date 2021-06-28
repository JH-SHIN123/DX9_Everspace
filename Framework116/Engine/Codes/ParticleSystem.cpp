#include "ParticleSystem.h"
#include "CollideSphere.h"
#include "Texture.h"
#include "Pipeline.h"

USING(Engine)

CParticleSystem::CParticleSystem(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CParticleSystem::CParticleSystem(const CParticleSystem& other)
	: CGameObject(other)
	, m_pVB(other.m_pVB)
	, m_vbSize(other.m_vbSize)
	, m_vbOffset(other.m_vbOffset)
	, m_vbBatchSize(other.m_vbBatchSize)
{
	m_pVB->AddRef();
}

bool CParticleSystem::IsEmpty_ParticleSystem()
{
	return m_listParticles.empty();
}

bool CParticleSystem::IsDead_ParticleSystem()
{
	for (auto& p : m_listParticles) {
		if (p.isAlive) return false;
	}

	return true;
}

void CParticleSystem::Reset_ParticleSystem()
{
	for (auto& p : m_listParticles)
	{
		ResetParticle_ParticleSystem(p);
	}
}

void CParticleSystem::AddParticle_ParticleSystem()
{
	PARTICLE_ATTRIBUTE attribute;
	ResetParticle_ParticleSystem(attribute);
	m_listParticles.emplace_back(attribute);
}

void CParticleSystem::RemoveDeadParticle_ParticleSystem()
{
	list<PARTICLE_ATTRIBUTE>::iterator iter;
	iter = m_listParticles.begin();

	while (iter != m_listParticles.end())
	{
		if (iter->isAlive == false) 
		{
			iter = m_listParticles.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

HRESULT CParticleSystem::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	// Particle 정점을 담을 버텍스버퍼 생성
	m_vbSize = 2048;
	m_vbOffset = 0;
	m_vbBatchSize = 512;

	if (FAILED(m_pDevice->CreateVertexBuffer(
		m_vbSize * sizeof(VTX_COLOR),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		FVF_VTX_COLOR,
		D3DPOOL_DEFAULT,
		&m_pVB,
		0
	))) 
	{
		PRINT_LOG(L"Error", L"Falied to CreateVertexBuffer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CParticleSystem::Ready_GameObject(void* pArg)
{
	CGameObject::Ready_GameObject(pArg);

	PARTICLESYSTEM_DESC* psDescPtr = nullptr;

	TRANSFORM_DESC transformDesc;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (psDescPtr = dynamic_cast<PARTICLESYSTEM_DESC*>(ptr))
		{
			transformDesc = psDescPtr->tTransformDesc;
			m_wstrTexturePrototypeTag = psDescPtr->wstrTexturePrototypeTag;
			m_iNumParticles = psDescPtr->iNumParticles;
			m_tResetAttribute = psDescPtr->tResetAttribute;
		}
	}

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform,
		(void*)&transformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		m_wstrTexturePrototypeTag,
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	return S_OK;
}

_uint CParticleSystem::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	return m_pTransform->Update_Transform();
}

_uint CParticleSystem::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	return _uint();
}

_uint CParticleSystem::Render_GameObject()
{
	if (false == m_listParticles.empty())
	{
		//PreRender_ParticleSystem();
		m_pDevice->SetRenderState(D3DRS_POINTSIZE, CPipeline::FtoDw(m_tResetAttribute.fParticleSize));
		
		m_pTexture->Set_Texture(0);
		m_pDevice->SetFVF(FVF_VTX_COLOR);
		m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTX_COLOR));

		//
		// render batches one by one
		//

		if (m_vbOffset >= m_vbSize)
			m_vbOffset = 0;

		VTX_COLOR* v = nullptr;

		m_pVB->Lock(
			m_vbOffset * sizeof(VTX_COLOR),
			m_vbBatchSize * sizeof(VTX_COLOR),
			(void**)&v,
			m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		//
		// Until all particles have been rendered.
		//

		for (auto& p : m_listParticles)
		{
			if (p.isAlive)
			{
				v->vPosition = p.vPos;
				v->iColor = p.tColor;
				++v;

				++numParticlesInBatch;

				// if this batch full?
				if (numParticlesInBatch == m_vbBatchSize)
				{
					//
					// Draw the last batch of particles that was
					// copied to the vertex buffer. 
					//
					m_pVB->Unlock();

					m_pDevice->DrawPrimitive(
						D3DPT_POINTLIST,
						m_vbOffset,
						m_vbBatchSize);
					//
					// While that batch is drawing, start filling the
					// next batch with particles.
					//

					// move the offset to the start of the next batch
					m_vbOffset += m_vbBatchSize;

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
					if (m_vbOffset >= m_vbSize)
						m_vbOffset = 0;

					m_pVB->Lock(
						m_vbOffset * sizeof(VTX_COLOR),
						m_vbBatchSize * sizeof(VTX_COLOR),
						(void**)&v,
						m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch
				}
			}
		}

		m_pVB->Unlock();

		// its possible that the LAST batch being filled never 
		// got rendered because the condition 
		// (numParticlesInBatch == _vbBatchSize) would not have
		// been satisfied.  We draw the last partially filled batch now.
		if (numParticlesInBatch)
		{
			m_pDevice->DrawPrimitive(
				D3DPT_POINTLIST,
				m_vbOffset,
				numParticlesInBatch);
		}

		// next block
		m_vbOffset += m_vbBatchSize;

		//
		// reset render states
		//

		//PostRender_ParticleSystem();
	}

	return _uint();
}


void CParticleSystem::Free()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	//Safe_Release(m_pCollide);

	m_listParticles.clear();

	CGameObject::Free();
}
