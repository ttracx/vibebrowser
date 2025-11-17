/**
 * Re-export VibeBrowser types as the primary configuration format
 * 
 * The new VibeBrowser provider configuration is now the primary format.
 * Legacy LLMSettings types have been removed in favor of the unified
 * VibeBrowserProvider structure.
 */
export { 
  VibeBrowserProvider,
  VibeBrowserProvidersConfig,
  VibeBrowserProviderType,
  VibeBrowserProviderSchema,
  VibeBrowserProvidersConfigSchema,
  VibeBrowserPrefObject,
  VibeBrowserPrefObjectSchema,
  ProviderCapabilitiesSchema,
  ModelConfigSchema,
  BROWSEROS_PREFERENCE_KEYS
} from './browserOSTypes' 