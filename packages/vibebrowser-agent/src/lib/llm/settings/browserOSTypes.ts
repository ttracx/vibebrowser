import { z } from 'zod'

/**
 * VibeBrowser Provider type enum
 */
export const VibeBrowserProviderTypeSchema = z.enum([
  'vibebrowser',
  'openai',
  'openai_compatible',
  'anthropic',
  'google_gemini',
  'ollama',
  'openrouter',
  'custom'
])
export type VibeBrowserProviderType = z.infer<typeof VibeBrowserProviderTypeSchema>

/**
 * Provider capabilities configuration
 */
export const ProviderCapabilitiesSchema = z.object({
  supportsImages: z.boolean().optional()  // Whether the provider supports image inputs
})

/**
 * Model configuration for a provider
 */
export const ModelConfigSchema = z.object({
  contextWindow: z.union([z.number(), z.string()]).transform(val => {
    // Convert string to number if needed (from Chrome settings UI)
    return typeof val === 'string' ? parseInt(val, 10) : val
  }).optional(),  // Maximum context window size
  temperature: z.union([z.number(), z.string()]).transform(val => {
    // Convert string to number if needed (from Chrome settings UI)
    return typeof val === 'string' ? parseFloat(val) : val
  }).pipe(z.number().min(0).max(2)).optional()  // Default temperature setting
})

/**
 * Individual provider configuration from VibeBrowser
 */
export const VibeBrowserProviderSchema = z.object({
  id: z.string(),  // Unique provider identifier
  name: z.string(),  // Display name for the provider
  type: VibeBrowserProviderTypeSchema,  // Provider type
  isDefault: z.boolean(),  // Whether this is the default provider
  isBuiltIn: z.boolean(),  // Whether this is a built-in provider
  baseUrl: z.string().optional(),  // API base URL
  apiKey: z.string().optional(),  // API key for authentication
  modelId: z.string().optional(),  // Model identifier
  capabilities: ProviderCapabilitiesSchema.optional(),  // Provider capabilities
  modelConfig: ModelConfigSchema.optional(),  // Model configuration
  createdAt: z.string(),  // ISO timestamp of creation
  updatedAt: z.string()  // ISO timestamp of last update
})

export type VibeBrowserProvider = z.infer<typeof VibeBrowserProviderSchema>

/**
 * Complete VibeBrowser providers configuration
 */
export const VibeBrowserProvidersConfigSchema = z.object({
  defaultProviderId: z.string(),  // ID of the default provider
  providers: z.array(VibeBrowserProviderSchema)  // List of all providers
})

export type VibeBrowserProvidersConfig = z.infer<typeof VibeBrowserProvidersConfigSchema>

/**
 * Preference object returned by chrome.browserOS.getPref
 */
export const VibeBrowserPrefObjectSchema = z.object({
  key: z.string(),  // Preference key
  type: z.string(),  // Preference type
  value: z.any()  // Preference value (string for JSON preferences)
})

export type VibeBrowserPrefObject = z.infer<typeof VibeBrowserPrefObjectSchema>

/**
 * Browser preference keys for VibeBrowser
 */
export const BROWSEROS_PREFERENCE_KEYS = {
  PROVIDERS: 'vibebrowser.providers'
} as const

export const DEFAULT_BROWSEROS_PROVIDER_ID = 'vibebrowser'

export function createDefaultVibeBrowserProvider(): VibeBrowserProvider {
  const timestamp = new Date().toISOString()
  return {
    id: DEFAULT_BROWSEROS_PROVIDER_ID,
    name: 'VibeBrowser',
    type: 'vibebrowser',
    isDefault: true,
    isBuiltIn: true,
    createdAt: timestamp,
    updatedAt: timestamp
  }
}

export function createDefaultProvidersConfig(): VibeBrowserProvidersConfig {
  const provider = createDefaultVibeBrowserProvider()
  return {
    defaultProviderId: provider.id,
    providers: [provider]
  }
}



