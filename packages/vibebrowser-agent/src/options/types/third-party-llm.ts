export interface ThirdPartyLLMProvider {
  id: string
  name: string
  url: string
  isBuiltIn?: boolean
}

export interface ThirdPartyLLMConfig {
  providers: Array<Pick<ThirdPartyLLMProvider, 'name' | 'url'>>
  selected_provider: number
}

export const DEFAULT_THIRD_PARTY_PROVIDERS: Array<Omit<ThirdPartyLLMProvider, 'id'>> = [
  { name: 'ChatGPT', url: 'https://chatgpt.com/', isBuiltIn: true },
  { name: 'Claude', url: 'https://claude.ai/', isBuiltIn: true },
  { name: 'Grok', url: 'https://grok.com/', isBuiltIn: true },
  { name: 'Gemini', url: 'https://gemini.google.com/', isBuiltIn: true },
  { name: 'Perplexity', url: 'https://www.perplexity.ai/', isBuiltIn: true },
  { name: 'AI Studio', url: 'https://aistudio.google.com/', isBuiltIn: true }
]

export const DEFAULT_THIRD_PARTY_CONFIG: ThirdPartyLLMConfig = {
  providers: DEFAULT_THIRD_PARTY_PROVIDERS.map(({ name, url }) => ({ name, url })),
  selected_provider: 0
}
