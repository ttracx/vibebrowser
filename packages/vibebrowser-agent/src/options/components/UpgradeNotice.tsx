import React from 'react'
import { AlertCircle, ExternalLink } from 'lucide-react'

interface UpgradeNoticeProps {
  featureName: string
  currentVersion: string | null
  requiredVersion: string
  className?: string
}

export function UpgradeNotice({
  featureName,
  currentVersion,
  requiredVersion,
  className = ''
}: UpgradeNoticeProps) {
  const handleUpgradeClick = () => {
    chrome.tabs.create({ url: 'https://github.com/vibebrowser-ai/VibeBrowser/releases/latest' })
  }

  return (
    <div className={`flex items-start gap-3 rounded-lg border border-l-4 border-purple-200 border-l-purple-500 bg-purple-50/50 p-4 ${className}`}>
      <AlertCircle className="w-5 h-5 flex-shrink-0 text-purple-500 mt-0.5" />

      <div className="flex-1 min-w-0">
        <p className="text-sm font-medium text-purple-900">
          VibeBrowser Update Required
        </p>
        <p className="mt-1 text-xs text-purple-700">
          {featureName} requires VibeBrowser v{requiredVersion} or higher.
          You're currently on {currentVersion || 'an older version'}.
        </p>
      </div>

      <button
        onClick={handleUpgradeClick}
        className="flex-shrink-0 inline-flex items-center gap-1.5 rounded-md border border-purple-500 bg-transparent px-3 py-1.5 text-xs font-medium text-purple-600 transition-colors hover:bg-purple-500 hover:text-white"
      >
        Update VibeBrowser
        <ExternalLink className="w-3 h-3" />
      </button>
    </div>
  )
}
