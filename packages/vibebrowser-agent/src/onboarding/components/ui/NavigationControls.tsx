import React from 'react'

interface NavigationControlsProps {
  onPrevious?: () => void
  onNext?: () => void
  onSkip?: () => void
  previousLabel?: string
  nextLabel?: string
  skipLabel?: string
  previousDisabled?: boolean
  nextDisabled?: boolean
  className?: string
  nextButtonPrimary?: boolean  // Use primary style for next button (e.g., final CTA)
}

export function NavigationControls({
  onPrevious,
  onNext,
  onSkip,
  previousLabel = 'Previous',
  nextLabel = 'Next',
  skipLabel = 'Skip',
  previousDisabled = false,
  nextDisabled = false,
  className,
  nextButtonPrimary = false
}: NavigationControlsProps) {
  if (!onPrevious && !onNext && !onSkip) {
    return null
  }

  const baseButtonClasses =
    'group flex items-center gap-2 px-8 py-3 backdrop-blur-md bg-purple-50/40 dark:bg-purple-950/40 border-2 border-purple-300/60 dark:border-purple-700/60 text-purple-600 dark:text-purple-400 font-bold rounded-lg transition-all duration-300 shadow-md shadow-purple-500/20 hover:bg-purple-100/50 dark:hover:bg-purple-900/50 hover:border-purple-400/70 dark:hover:border-purple-600/70 hover:shadow-lg hover:shadow-purple-500/30 hover:scale-105 active:scale-95 disabled:cursor-not-allowed disabled:opacity-60 disabled:shadow-none disabled:translate-y-0'

  const skipButtonClasses =
    'px-8 py-3 backdrop-blur-md bg-muted/40 border-2 border-border/60 text-muted-foreground font-semibold rounded-lg transition-all duration-300 hover:bg-muted/60 hover:border-border hover:scale-105 active:scale-95'

  const primaryButtonClasses =
    'group relative px-8 py-3 bg-gradient-to-r from-brand to-purple-500 hover:from-brand/90 hover:to-purple-500/90 text-white font-bold rounded-lg transition-all duration-300 shadow-lg shadow-brand/25 hover:shadow-xl hover:shadow-brand/40 hover:scale-105 active:scale-95 overflow-hidden flex items-center gap-2'

  return (
    <div className={`flex justify-between items-center ${className ?? 'pt-4'}`}>
      {onPrevious && (
        <button
          type="button"
          onClick={onPrevious}
          disabled={previousDisabled}
          className={baseButtonClasses}
        >
          <svg
            className="w-4 h-4 transition-transform duration-200 group-hover:-translate-x-1"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M15 19l-7-7 7-7" />
          </svg>
          {previousLabel}
        </button>
      )}

      {/* Only use wrapper div if skip button exists */}
      {onSkip ? (
        <div className="flex gap-3">
          <button
            type="button"
            onClick={onSkip}
            className={skipButtonClasses}
          >
            {skipLabel}
          </button>
          {onNext && (
            <button
              type="button"
              onClick={onNext}
              disabled={nextDisabled}
              className={nextButtonPrimary ? primaryButtonClasses : baseButtonClasses}
            >
              {nextLabel}
              <svg
                className="w-4 h-4 transition-transform duration-300 group-hover:translate-x-1"
                fill="none"
                stroke="currentColor"
                viewBox="0 0 24 24"
              >
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 5l7 7-7 7" />
              </svg>
              {nextButtonPrimary && (
                <div className="absolute inset-0 bg-gradient-to-r from-transparent via-white/10 to-transparent -translate-x-full group-hover:translate-x-full transition-transform duration-1000" />
              )}
            </button>
          )}
        </div>
      ) : (
        onNext && (
          <button
            type="button"
            onClick={onNext}
            disabled={nextDisabled}
            className={nextButtonPrimary ? primaryButtonClasses : baseButtonClasses}
          >
            {nextLabel}
            <svg
              className="w-4 h-4 transition-transform duration-300 group-hover:translate-x-1"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 5l7 7-7 7" />
            </svg>
            {nextButtonPrimary && (
              <div className="absolute inset-0 bg-gradient-to-r from-transparent via-white/10 to-transparent -translate-x-full group-hover:translate-x-full transition-transform duration-1000" />
            )}
          </button>
        )
      )}
    </div>
  )
}
