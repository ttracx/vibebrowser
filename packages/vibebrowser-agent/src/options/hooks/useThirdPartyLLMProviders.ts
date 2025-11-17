import { useCallback, useEffect, useState } from 'react'
import { getVibeBrowserAdapter } from '@/lib/browser/VibeBrowserAdapter'
import {
  DEFAULT_THIRD_PARTY_CONFIG,
  DEFAULT_THIRD_PARTY_PROVIDERS,
  ThirdPartyLLMConfig,
  ThirdPartyLLMProvider
} from '../types/third-party-llm'

type ProviderInput = Pick<ThirdPartyLLMProvider, 'name' | 'url'>

// VibeBrowser preference keys for third-party LLM providers
const PROVIDERS_PREF_KEY = 'vibebrowser.third_party_llm.providers'
const SELECTED_PROVIDER_PREF_KEY = 'vibebrowser.third_party_llm.selected_provider'

interface UseThirdPartyLLMProvidersResult {
  providers: ThirdPartyLLMProvider[]
  selectedProviderId: string | null
  isLoading: boolean
  isSaving: boolean
  error: string | null
  isVibeBrowser: boolean
  addProvider: (provider: ProviderInput) => Promise<void>
  updateProvider: (id: string, provider: ProviderInput) => Promise<void>
  deleteProvider: (id: string) => Promise<void>
  setSelectedProvider: (id: string) => Promise<void>
  refresh: () => Promise<void>
}

function createHashId(name: string, url: string, index: number): string {
  const trimmedName = name.trim()
  const trimmedUrl = url.trim()

  // Use fallback for empty or invalid inputs
  if (!trimmedName || !trimmedUrl) {
    if (typeof crypto !== 'undefined' && typeof crypto.randomUUID === 'function') {
      return crypto.randomUUID()
    }
    return `providers-hub-${Date.now()}-${Math.random().toString(16).slice(2)}`
  }

  // Generate hash from valid inputs
  const normalized = `${trimmedName.toLowerCase()}|${trimmedUrl.toLowerCase()}`
  let hash = 0
  for (let i = 0; i < normalized.length; i++) {
    hash = (hash * 31 + normalized.charCodeAt(i)) >>> 0
  }
  return `providers-hub-${hash.toString(16)}-${index}`
}

function ensureProtocol(url: string): string {
  const trimmed = url.trim()
  if (!trimmed) return trimmed
  if (/^[a-zA-Z][a-zA-Z0-9+.-]*:/.test(trimmed)) {
    return trimmed
  }
  return `https://${trimmed}`
}

function normalizeUrlForComparison(url: string): string {
  try {
    const normalized = ensureProtocol(url)
    const parsed = new URL(normalized)
    const pathname = parsed.pathname.replace(/\/+$/, '')
    return `${parsed.origin}${pathname}`
  } catch {
    return url.trim().toLowerCase()
  }
}

function mapToProviderList(entries: Array<{ name?: unknown; url?: unknown }>): ThirdPartyLLMProvider[] {
  return entries
    .map((entry, index) => {
      const name = typeof entry.name === 'string' ? entry.name.trim() : ''
      const url = typeof entry.url === 'string' ? entry.url.trim() : ''
      if (!name || !url) return null

      const builtIn = DEFAULT_THIRD_PARTY_PROVIDERS.some(defaultProvider => {
        const sameName = defaultProvider.name.trim().toLowerCase() === name.toLowerCase()
        const sameUrl = normalizeUrlForComparison(defaultProvider.url) === normalizeUrlForComparison(url)
        return sameName && sameUrl
      })

      return {
        id: createHashId(name, url, index),
        name,
        url,
        isBuiltIn: builtIn
      } as ThirdPartyLLMProvider
    })
    .filter((provider): provider is ThirdPartyLLMProvider => Boolean(provider))
}

function parseProvidersValue(raw: unknown): Array<{ name: string; url: string }> | null {
  try {
    if (raw == null) {
      return null
    }

    let data: unknown = raw

    if (typeof data === 'string') {
      const trimmed = data.trim()
      if (!trimmed) {
        return null
      }
      try {
        data = JSON.parse(trimmed)
      } catch (e) {
        console.error('[ProvidersHub] Failed to parse providers JSON string:', e)
        return null
      }
    }

    if (Array.isArray(data)) {
      const providers = data
        .map(item => {
          const name = typeof item?.name === 'string' ? item.name : ''
          const url = typeof item?.url === 'string' ? item.url : ''
          return name && url ? { name, url } : null
        })
        .filter((entry): entry is { name: string; url: string } => Boolean(entry))

      return providers.length > 0 ? providers : null
    }

    if (typeof data === 'object' && data !== null && Array.isArray((data as any).providers)) {
      return parseProvidersValue((data as any).providers)
    }

    return null
  } catch (error) {
    console.error('[ProvidersHub] Error parsing providers value:', error)
    return null
  }
}

function buildConfigFromPrefs(providersValue: unknown, selectedValue: unknown): ThirdPartyLLMConfig | null {
  const providers = parseProvidersValue(providersValue)
  if (!providers) {
    return null
  }

  let selectedProvider = 0

  if (typeof selectedValue === 'number' && Number.isFinite(selectedValue)) {
    selectedProvider = selectedValue
  } else if (typeof selectedValue === 'string') {
    const parsed = Number.parseInt(selectedValue, 10)
    if (Number.isFinite(parsed)) {
      selectedProvider = parsed
    }
  }

  selectedProvider = Math.max(0, Math.min(selectedProvider, providers.length - 1))

  return {
    providers,
    selected_provider: selectedProvider
  }
}

export function useThirdPartyLLMProviders(): UseThirdPartyLLMProvidersResult {
  const [providers, setProviders] = useState<ThirdPartyLLMProvider[]>([])
  const [selectedProviderId, setSelectedProviderId] = useState<string | null>(null)
  const [isLoading, setIsLoading] = useState(true)
  const [isSaving, setIsSaving] = useState(false)
  const [error, setError] = useState<string | null>(null)
  const [isVibeBrowser, setIsVibeBrowser] = useState(false)
  const [refreshCounter, setRefreshCounter] = useState(0)

  // Load providers from VibeBrowser preferences
  useEffect(() => {
    let mounted = true

    const loadProviders = async () => {
      const adapter = getVibeBrowserAdapter()

      try {
        const [providersPrefResult, selectedPrefResult] = await Promise.allSettled([
          adapter.getPref(PROVIDERS_PREF_KEY),
          adapter.getPref(SELECTED_PROVIDER_PREF_KEY)
        ])

        if (!mounted) return

        const providersPref =
          providersPrefResult.status === 'fulfilled' ? providersPrefResult.value : null
        const selectedPref =
          selectedPrefResult.status === 'fulfilled' ? selectedPrefResult.value : null

        const config = buildConfigFromPrefs(providersPref?.value, selectedPref?.value)

        if (config && config.providers.length > 0) {
          const loadedProviders = mapToProviderList(config.providers)
          const selectedIndex = Math.max(
            0,
            Math.min(config.selected_provider, loadedProviders.length - 1)
          )

          setProviders(loadedProviders)
          setSelectedProviderId(loadedProviders[selectedIndex]?.id ?? null)
          setIsVibeBrowser(true)
          setError(null)
          setIsLoading(false)
          return
        }

        if (providersPrefResult.status === 'fulfilled') {
          const providersPayload = DEFAULT_THIRD_PARTY_CONFIG.providers.map(({ name, url }) => ({
            name: name.trim(),
            url: url.trim()
          }))

          const [providersSuccess, selectedSuccess] = await Promise.all([
            adapter.setPref(PROVIDERS_PREF_KEY, providersPayload),
            adapter.setPref(SELECTED_PROVIDER_PREF_KEY, DEFAULT_THIRD_PARTY_CONFIG.selected_provider)
          ])

          if (!mounted) return

          if (providersSuccess && selectedSuccess) {
            const defaultProviders = mapToProviderList(DEFAULT_THIRD_PARTY_CONFIG.providers)
            const defaultSelectedIndex = Math.max(
              0,
              Math.min(
                DEFAULT_THIRD_PARTY_CONFIG.selected_provider,
                defaultProviders.length - 1
              )
            )
            setProviders(defaultProviders)
            setSelectedProviderId(defaultProviders[defaultSelectedIndex]?.id ?? null)
            setIsVibeBrowser(true)
            setError(null)
            return
          }

          throw new Error('Failed to persist default providers preferences')
        }

        throw new Error('VibeBrowser providers preference unavailable')
      } catch (err) {
        console.error('[ProvidersHub] Error loading providers:', err)

        if (!mounted) return

        // Fallback to demo providers on error
        const defaults = mapToProviderList(DEFAULT_THIRD_PARTY_PROVIDERS)
        setProviders(defaults)
        setSelectedProviderId(defaults[0]?.id ?? null)
        setIsVibeBrowser(false)
        setError('This feature requires VibeBrowser browser. Showing demo providers.')
      } finally {
        if (mounted) {
          setIsLoading(false)
        }
      }
    }

    loadProviders()

    return () => {
      mounted = false
    }
  }, [refreshCounter])

  const persistConfig = useCallback(
    async (updatedProviders: ThirdPartyLLMProvider[], selectedId: string | null) => {
      const selectedIndex = updatedProviders.findIndex(p => p.id === selectedId)
      const safeSelectedIndex = selectedIndex >= 0 ? selectedIndex : 0

      const adapter = getVibeBrowserAdapter()
      const providersPayload = updatedProviders.map(p => ({
        name: p.name.trim(),
        url: p.url.trim()
      }))

      try {
        const [providersSuccess, selectedSuccess] = await Promise.all([
          adapter.setPref(PROVIDERS_PREF_KEY, providersPayload),
          adapter.setPref(SELECTED_PROVIDER_PREF_KEY, safeSelectedIndex)
        ])

        if (!providersSuccess || !selectedSuccess) {
          throw new Error('setPref returned false')
        }
      } catch (err) {
        console.error('[ProvidersHub] Error persisting config:', err)
        throw err
      }
    },
    []
  )

  const addProvider = useCallback(async (provider: ProviderInput) => {
    if (!isVibeBrowser) {
      throw new Error('This feature requires VibeBrowser browser')
    }

    const name = provider.name.trim()
    const url = ensureProtocol(provider.url)

    if (!name) {
      throw new Error('Provider name is required')
    }

    if (!url) {
      throw new Error('Provider URL is required')
    }

    let parsedUrl: URL | null = null
    try {
      parsedUrl = new URL(url)
    } catch {
      throw new Error('Please enter a valid URL (example: https://example.com)')
    }

    const newProvider: ThirdPartyLLMProvider = {
      id: createHashId(name, parsedUrl.toString(), providers.length + 1),
      name,
      url: parsedUrl.toString(),
      isBuiltIn: false
    }

    const previousProviders = providers
    const previousSelectedId = selectedProviderId
    const updatedProviders = [...providers, newProvider]
    const updatedSelectedId = previousProviders.length === 0 ? newProvider.id : previousSelectedId

    setIsSaving(true)
    setProviders(updatedProviders)
    if (!previousSelectedId) {
      setSelectedProviderId(updatedSelectedId)
    }

    try {
      await persistConfig(updatedProviders, updatedSelectedId ?? newProvider.id)
      if (!previousSelectedId) {
        setSelectedProviderId(updatedSelectedId ?? newProvider.id)
      }
    } catch (error) {
      setProviders(previousProviders)
      setSelectedProviderId(previousSelectedId ?? null)
      setIsSaving(false)
      throw error
    }

    setIsSaving(false)
  }, [persistConfig, providers, selectedProviderId, isVibeBrowser])

  const updateProvider = useCallback(async (id: string, provider: ProviderInput) => {
    if (!isVibeBrowser) {
      throw new Error('This feature requires VibeBrowser browser')
    }

    const name = provider.name.trim()
    const url = ensureProtocol(provider.url)

    if (!name) {
      throw new Error('Provider name is required')
    }

    if (!url) {
      throw new Error('Provider URL is required')
    }

    try {
      // Validate URL
      new URL(url)
    } catch {
      throw new Error('Please enter a valid URL (example: https://example.com)')
    }

    const previousProviders = providers
    const updatedProviders = providers.map(existing => {
      if (existing.id !== id) return existing
      return {
        ...existing,
        name,
        url
      }
    })

    setIsSaving(true)
    setProviders(updatedProviders)

    try {
      await persistConfig(updatedProviders, selectedProviderId)
    } catch (error) {
      setProviders(previousProviders)
      setIsSaving(false)
      throw error
    }

    setIsSaving(false)
  }, [persistConfig, providers, selectedProviderId, isVibeBrowser])

  const deleteProvider = useCallback(async (id: string) => {
    if (!isVibeBrowser) {
      throw new Error('This feature requires VibeBrowser browser')
    }

    if (providers.length <= 1) {
      throw new Error('At least one provider must remain configured')
    }

    const provider = providers.find(item => item.id === id)
    if (!provider) return

    const previousProviders = providers
    const previousSelectedId = selectedProviderId

    const updatedProviders = providers.filter(item => item.id !== id)

    let nextSelectedId = previousSelectedId
    if (previousSelectedId === id) {
      nextSelectedId = updatedProviders[0]?.id ?? null
    }

    setIsSaving(true)
    setProviders(updatedProviders)
    setSelectedProviderId(nextSelectedId ?? null)

    try {
      await persistConfig(updatedProviders, nextSelectedId)
    } catch (error) {
      setProviders(previousProviders)
      setSelectedProviderId(previousSelectedId ?? null)
      setIsSaving(false)
      throw error
    }

    setIsSaving(false)
  }, [persistConfig, providers, selectedProviderId, isVibeBrowser])

  const setSelectedProvider = useCallback(async (id: string) => {
    if (!isVibeBrowser) {
      throw new Error('This feature requires VibeBrowser browser')
    }

    if (id === selectedProviderId) return

    if (!providers.some(provider => provider.id === id)) {
      throw new Error('Provider not found')
    }

    const previousSelectedId = selectedProviderId
    setIsSaving(true)
    setSelectedProviderId(id)

    try {
      await persistConfig(providers, id)
    } catch (error) {
      setSelectedProviderId(previousSelectedId ?? null)
      setIsSaving(false)
      throw error
    }

    setIsSaving(false)
  }, [persistConfig, providers, selectedProviderId, isVibeBrowser])

  const refresh = useCallback(async () => {
    setRefreshCounter(prev => prev + 1)
  }, [])

  return {
    providers,
    selectedProviderId,
    isLoading,
    isSaving,
    error,
    isVibeBrowser,
    addProvider,
    updateProvider,
    deleteProvider,
    setSelectedProvider,
    refresh
  }
}
